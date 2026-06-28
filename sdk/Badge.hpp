// Originally written by Luca Courte. The LED matrix driver was substantially
// rewritten by Eljakim Schrijvers.
#pragma once

#include <cstdint>

enum class Key {
    Up,
    Down,
    Left,
    Right
};

// The badge's 20x12 LED matrix. Every pixel has a brightness 0..255.
//
// The display is refreshed in the BACKGROUND by a hardware timer interrupt, not
// by your program's loop. You draw a frame with setPixel(...) and call
// updatePixels() to publish it; from then on the interrupt keeps the panel lit
// and the grayscale steady, all on its own. Two consequences you asked for:
//
//   * Your frame rate is yours.   Update once a second or a thousand times a
//     second, the picture never flickers and brightness keeps working, because
//     the refresh runs at its own fixed rate regardless of how often you draw.
//   * It sips power.   Between refresh interrupts the CPU sleeps (and runs at a
//     low clock), instead of busy-spinning the matrix the way it used to.
//
//   Badge badge;
//   badge.clearPixels();
//   badge.setPixel(10, 6, 255);   // full brightness
//   badge.setPixel(11, 6, 64);    // dim
//   badge.updatePixels();         // publish; the background refresh does the rest
//
// Plain on/off still works: setPixel(x, y) lights a pixel fully.
//
// For a whole-panel dimmer (one brightness for everything, done in hardware on
// the output-enable pin) see KimsPower::setBrightness. That composes with the
// per-LED brightness here.
class Badge final {
public:
    Badge();
    ~Badge();

    Badge(const Badge&) = delete;
    Badge(Badge&&) = delete;
    Badge& operator=(const Badge&) = delete;
    Badge& operator=(Badge&&) = delete;

    void clearPixels();
    void setPixel(unsigned x, unsigned y, unsigned brightness = 255);
    void updatePixels();

    bool isPressed(Key key) const;

    // Pace your loop. updatePixels() waits this many milliseconds per frame so the
    // CPU can idle (low power) between frames while the background refresh keeps
    // the display lit. This sets your frame rate; it has no effect on the display
    // refresh or on per-LED brightness. Default 10 ms (~100 fps). 0 = no wait.
    void setFrameDelay(unsigned ms);

private:
    uint8_t  _framebuffer[12][20];
    unsigned _frameDelayMs = 10;
};
