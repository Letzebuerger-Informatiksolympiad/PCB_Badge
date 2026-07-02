// Originally written by Luca Courte. The LED matrix driver was substantially
// rewritten by Eljakim Schrijvers.
#include "Badge.hpp"

#include <cmath>

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/timer.h>
#include <hardware/vreg.h>
#include <pico/stdlib.h>

// How this driver works
// ─────────────────────
// The matrix is column-multiplexed through a 32-bit shift register: one column
// is pulled low at a time and the 12 row lines drive that column's pixels. To get
// PER-LED brightness we use Binary Code Modulation (BCM): within a column we show
// BITS bit-planes back to back, plane p held 2^p time units, and a pixel joins
// the planes whose bits are set in its level (the gamma-mapped brightness, see
// g_level). Averaged by the eye, that is its brightness.
//
// The whole refresh (20 columns * BITS planes) is precomputed into 32-bit shift
// words and walked by a HARDWARE TIMER INTERRUPT, one word per IRQ. So the matrix
// keeps refreshing on its own; the foreground only rebuilds the words when it has
// a new frame (updatePixels). Two buffers, swapped atomically, keep that tear-free
// and let the foreground run, or sleep, at any pace.

namespace {

// ── Pins ─────────────────────────────────────────
const uint BTN_UP    = 0;
const uint BTN_RIGHT = 1;
const uint BTN_LEFT  = 2;
const uint BTN_DOWN  = 3;

const uint DIN  = 9;
const uint OE   = 10;
const uint CLK  = 11;
const uint RCLK = 12;

// ── Bit maps ─────────────────────────────────────
constexpr uint8_t COL_BIT[20] = {
  11,10,9,23,22,21,20,19,18,17,
  31,30,29,28,27,26,25,24,8,16
};
constexpr uint8_t ROW_BIT[12] = {
  12,13,14,15,0,4,3,2,1,5,6,7
};

// Every column line HIGH = whole panel blanked. Computed once at compile time.
constexpr uint32_t allColumnsOff() {
    uint32_t m = 0;
    for (int c = 0; c < 20; c++) m |= (1UL << COL_BIT[c]);
    return m;
}
constexpr uint32_t ALL_COLS_OFF = allColumnsOff();

// ── BCM settings ─────────────────────────────────
// BITS bit-planes give 2^BITS shades. UNIT_US is the least-significant plane's
// on-time; brighter planes hold 2x, 4x, ... that. A full refresh is
// 20 * UNIT_US * (2^BITS - 1) ~ 3.8 ms of lit time (plus a little shift time);
// ~230 Hz, steady to the eye even for the dimmest pixels.
//
// 6 bits = 64 shades, which (with the gamma curve below) gives real resolution
// down at the dim end: a pixel can sit at ~1% on instead of jumping straight from
// off to ~12%. That dim end is exactly where the eye is most sensitive.
constexpr int COLS    = 20;
constexpr int ROWS    = 12;
constexpr int BITS    = 6;
constexpr int UNIT_US = 3;
constexpr int STEPS   = COLS * BITS;          // (column, plane) states per refresh
constexpr int MAXLEVEL = (1 << BITS) - 1;     // brightest BCM level (63)

// Perceptual gamma. An LED's apparent brightness is nowhere near its on-fraction:
// 12% duty already looks "mostly on". GAMMA bends the 0..255 you pass to setPixel
// so that equal steps look equal and small values stay genuinely dim. 2.2 is the
// usual display gamma. The gamma curve is baked into a lookup table at startup.
constexpr float GAMMA = 2.2f;
uint8_t g_level[256];                          // setPixel value (0..255) -> BCM level (0..MAXLEVEL)

// Shift 32 bits out MSB-first and latch. No per-bit delay: back-to-back gpio_put
// calls are well within the '595's shift-clock limits.
//
// We blank the panel (OE high) while shifting and unblank after the latch. That
// way each plane is lit for exactly its hold time, with no "bleed" from the old
// word still showing during the ~microseconds it takes to clock the new one out.
// It keeps the brightness ratios clean on the real board -- and makes the emulator
// match it. (If KimsPower has taken OE over for a global dimmer, the OE writes are
// ignored and we simply fall back to no blanking.)
inline void write32(uint32_t value) {
    gpio_put(OE, 1);                  // blank while shifting
    for (int i = 31; i >= 0; i--) {
        gpio_put(DIN, (value >> i) & 1u);
        gpio_put(CLK, 1);
        gpio_put(CLK, 0);
    }
    gpio_put(RCLK, 1);
    gpio_put(RCLK, 0);
    gpio_put(OE, 0);                  // show the freshly latched word
}

inline void initOutput(uint pin) { gpio_init(pin); gpio_set_dir(pin, GPIO_OUT); }
inline void initButton(uint pin) { gpio_init(pin); gpio_set_dir(pin, GPIO_IN); gpio_pull_up(pin); }

// Hold time for the plane shown at a given step (step = col*BITS + plane).
inline uint32_t holdUs(int step) {
    return (uint32_t)UNIT_US << (step % BITS);
}

// ── Background refresh state ──────────────────────
// The IRQ reads words[active]; updatePixels() fills words[active ^ 1] then flips
// `active`. That flip is a single aligned 32-bit store, atomic on the Cortex-M0+,
// so the IRQ always sees one whole consistent frame. `step` is owned by the IRQ.
struct Refresh {
    volatile uint32_t words[2][STEPS];
    volatile int      active;
    volatile int      step;
    alarm_id_t        alarm;
};
Refresh g_rf = { {{0}}, 0, 0, -1 };

// The refresh state above is one global shared with the timer IRQ, so only one
// Badge may drive it: a second instance would add a second alarm that also
// advances `step` (corrupting the plane timing), and either destructor would
// blank the display for the survivor.
bool g_badgeExists = false;

// Fired by the timer alarm: show one (column, plane), then re-arm for its hold.
int64_t alarmCb(alarm_id_t, void*) {
    const int s = g_rf.step;
    write32(g_rf.words[g_rf.active][s]);
    g_rf.step = (s + 1 == STEPS) ? 0 : s + 1;
    // A positive return re-arms holdUs after this callback RETURNS (that is what
    // the sign means to the Pico SDK). The hold has to start only once write32 has
    // finished shifting: the shift itself takes ~10 us, longer than the shortest
    // plane holds, so the hold must not include it.
    return (int64_t)holdUs(s);
}

}  // namespace

Badge::Badge() {
    if (g_badgeExists) panic("Badge: only one Badge object may exist at a time");
    g_badgeExists = true;

    // Battery first. A non-stop matrix refresh means the CPU never gets to sleep
    // deeply, so we run the core slow and low-voltage (the same move KimsPower
    // makes). The microsecond timer that paces the refresh is independent of the
    // system clock, so the BCM timing below is unaffected.
    vreg_set_voltage(VREG_VOLTAGE_0_90);
    sleep_ms(2);                       // let the voltage settle before slowing down
    set_sys_clock_khz(48000, true);

    initOutput(DIN);
    initOutput(CLK);
    initOutput(RCLK);
    initOutput(OE);
    gpio_put(OE, 0);                   // output-enable active (KimsPower may PWM this)

    initButton(BTN_UP);
    initButton(BTN_DOWN);
    initButton(BTN_LEFT);
    initButton(BTN_RIGHT);

    // Build the gamma table once: setPixel value 0..255 -> BCM level 0..MAXLEVEL,
    // bent so equal value steps look like equal brightness steps and small values
    // stay dim. round() so e.g. value 255 maps to the brightest level.
    for (int v = 0; v < 256; v++) {
        float duty = powf(v / 255.0f, GAMMA);
        int   lvl  = (int)(duty * MAXLEVEL + 0.5f);
        g_level[v] = (uint8_t)(lvl < 0 ? 0 : lvl > MAXLEVEL ? MAXLEVEL : lvl);
    }

    clearPixels();
    for (int s = 0; s < STEPS; s++) {
        g_rf.words[0][s] = ALL_COLS_OFF;
        g_rf.words[1][s] = ALL_COLS_OFF;
    }
    g_rf.active = 0;
    g_rf.step   = 0;
    write32(ALL_COLS_OFF);             // start blanked

    // Hand the matrix to the hardware timer. From here it refreshes itself.
    g_rf.alarm = add_alarm_in_us(holdUs(0), alarmCb, nullptr, true);
}

Badge::~Badge() {
    if (g_rf.alarm >= 0) {
        cancel_alarm(g_rf.alarm);
        g_rf.alarm = -1;
    }
    write32(ALL_COLS_OFF);
    g_badgeExists = false;
}

void Badge::clearPixels() {
    for (int y = 0; y < ROWS; y++)
        for (int x = 0; x < COLS; x++)
            _framebuffer[y][x] = 0;
}

void Badge::setPixel(unsigned x, unsigned y, unsigned brightness) {
    if (x >= COLS || y >= ROWS) return;
    if (brightness > 255) brightness = 255;
    _framebuffer[y][x] = (uint8_t)brightness;
}

void Badge::updatePixels() {
    // Build this frame's BCM words into the spare buffer, then publish with one
    // atomic store. The refresh IRQ keeps running the old buffer until it lands.
    const int buf = g_rf.active ^ 1;
    for (int col = 0; col < COLS; col++) {
        const uint32_t colWord = ALL_COLS_OFF & ~(1UL << COL_BIT[col]);

        // One pass over the rows builds every bit-plane's lit rows for this column.
        uint32_t planeRows[BITS] = {0};
        for (int row = 0; row < ROWS; row++) {
            const unsigned level  = g_level[_framebuffer[row][col]];   // gamma-corrected
            const uint32_t rowBit = 1UL << ROW_BIT[row];
            for (int p = 0; p < BITS; p++)
                if (level & (1u << p)) planeRows[p] |= rowBit;
        }
        for (int p = 0; p < BITS; p++)
            g_rf.words[buf][col * BITS + p] = colWord | planeRows[p];
    }
    g_rf.active = buf;                 // atomic publish

    // Pace the foreground so the CPU idles between frames (the refresh IRQ keeps
    // the display alive meanwhile). This is your frame rate, not the display's.
    if (_frameDelayMs) sleep_ms(_frameDelayMs);
}

void Badge::setFrameDelay(unsigned ms) {
    _frameDelayMs = ms;
}

bool Badge::isPressed(Key key) const {
    switch (key) {
        case Key::Up:    return !gpio_get(BTN_UP);
        case Key::Down:  return !gpio_get(BTN_DOWN);
        case Key::Left:  return !gpio_get(BTN_LEFT);
        case Key::Right: return !gpio_get(BTN_RIGHT);
        default:         return false;
    }
}
