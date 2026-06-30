// example by Eljakim Schrijvers, sorry for going crazy
// A countdown timer that scales its own font to fit the 20-wide panel. While you
// set/edit it the hour is always shown: H:MM:SS in a tall 3x7 font, dropping to a
// packed HH:MM:SS once the hour is two digits. While running with a zero hour it
// drops the hour and shows MM:SS in a big 4x7 font.
//   Right starts it. Left begins editing (seconds blink; Up/Down change the field
//   rolling 00<->59, each Left moves seconds->minutes->hours). While running any
//   button pauses; paused, Right resets and anything else resumes. At zero it
//   flashes until a button is pressed. Leading zeros are a dim gray and the colons
//   breathe in a soft pulsing gray.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "trig.hpp"
#include <pico/stdlib.h>

namespace {

const int HT  = 7;    // both scalable fonts are 7 tall
const int DIM = 80;   // gray for leading zeros

struct Font { int w; const char* rows[10][HT]; };

const Font F3 = { 3, {
    {"###","# #","# #","# #","# #","# #","###"},
    {" # ","## "," # "," # "," # "," # ","###"},
    {"###","  #","  #","###","#  ","#  ","###"},
    {"###","  #","  #","###","  #","  #","###"},
    {"# #","# #","# #","###","  #","  #","  #"},
    {"###","#  ","#  ","###","  #","  #","###"},
    {"###","#  ","#  ","###","# #","# #","###"},
    {"###","  #","  #"," # "," # "," # "," # "},
    {"###","# #","# #","###","# #","# #","###"},
    {"###","# #","# #","###","  #","  #","###"},
}};

const Font F4 = { 4, {
    {".##.","#..#","#..#","#..#","#..#","#..#",".##."},
    {".#..","##..",".#..",".#..",".#..",".#..","###."},
    {".##.","#..#","...#","..#.",".#..","#...","####"},
    {"####","...#","..#.",".##.","...#","#..#",".##."},
    {"#..#","#..#","#..#","####","...#","...#","...#"},
    {"####","#...","###.","...#","...#","#..#",".##."},
    {".##.","#...","#...","###.","#..#","#..#",".##."},
    {"####","...#","..#.","..#.",".#..",".#..",".#.."},
    {".##.","#..#","#..#",".##.","#..#","#..#",".##."},
    {".##.","#..#","#..#",".###","...#","...#",".##."},
}};

void drawGlyph(Badge& b, const Font& f, int x, int y, int d, int bright) {
    for (int r = 0; r < HT; r++)
        for (int c = 0; c < f.w; c++)
            if (f.rows[d][r][c] == '#') b.setPixel(x + c, y + r, bright);
}

// Draw a row of digits. flds is each digit's time field (0=sec,1=min,2=hour):
// field==hideField is blanked (blink off-phase), field==editField stays bright,
// and digits before the first significant one are dimmed (leading zeros). Colons
// (two pulsing dots) go at the colx columns.
void drawSeq(Badge& b, const Font& f, int y, const int* digs, const int* xs,
             const int* flds, int n, const int* colx, int ncol,
             int hideField, int editField, int colon) {
    int firstSig = n - 1;
    for (int i = 0; i < n; i++) if (digs[i] != 0) { firstSig = i; break; }
    for (int i = 0; i < n; i++) {
        if (hideField == flds[i]) continue;
        bool bright = i >= firstSig || flds[i] == editField;
        drawGlyph(b, f, xs[i], y, digs[i], bright ? 255 : DIM);
    }
    for (int i = 0; i < ncol; i++) {
        b.setPixel(colx[i], y + HT / 3, colon);
        b.setPixel(colx[i], y + 2 * HT / 3, colon);
    }
}

// Pick the layout from the hour. allowBig drops a zero hour for the big MM:SS;
// false (setting/editing) always shows the hour so it can be edited.
void render(Badge& b, int H, int M, int S, int hideField, int editField, int colon, bool allowBig) {
    const int y = 2;
    if (allowBig && H == 0) {                          // MM:SS, big 4x7
        int digs[4] = { M / 10, M % 10, S / 10, S % 10 };
        int xs[4]   = { 0, 5, 10, 15 };
        int flds[4] = { 1, 1, 0, 0 };
        int colx[1] = { 9 };
        drawSeq(b, F4, y, digs, xs, flds, 4, colx, 1, hideField, editField, colon);
    } else if (H <= 9) {                               // H:MM:SS, 3x7 with gaps
        int digs[5] = { H, M / 10, M % 10, S / 10, S % 10 };
        int xs[5]   = { 0, 4, 8, 12, 16 };
        int flds[5] = { 2, 1, 1, 0, 0 };
        int colx[2] = { 3, 11 };
        drawSeq(b, F3, y, digs, xs, flds, 5, colx, 2, hideField, editField, colon);
    } else {                                           // HH:MM:SS, 3x7 packed
        int digs[6] = { H / 10, H % 10, M / 10, M % 10, S / 10, S % 10 };
        int xs[6]   = { 0, 3, 7, 10, 14, 17 };
        int flds[6] = { 2, 2, 1, 1, 0, 0 };
        int colx[2] = { 6, 13 };
        drawSeq(b, F3, y, digs, xs, flds, 6, colx, 2, hideField, editField, colon);
    }
}

}  // namespace

class Timer : public MainLoop {
public:
    Timer(Badge& badge) : MainLoop(badge) {
        badge.setFrameDelay(30);   // responsive for the buttons, easy on the battery
    }

    void nextFrame(Badge& badge, const FrameData& data) override {
        uint32_t now = to_ms_since_boot(get_absolute_time());
        const auto& btn = data.buttons;
        bool any = btn.up.pressed || btn.down.pressed || btn.left.pressed || btn.right.pressed;

        int colon = 90 + isin(now / 6) * 50 / 256;   // colons breathe, dim<->light gray

        switch (mode) {
            case SET:
                if (btn.right.pressed) start(now);
                else if (btn.left.pressed) { mode = EDIT; field = 0; }
                render(badge, h, m, s, -1, -1, colon, false);
                break;

            case EDIT:
                if (btn.right.pressed) { start(now); break; }
                if (btn.left.pressed) field = (field + 1) % 3;
                if (btn.up.pressed)   bump(+1);
                if (btn.down.pressed) bump(-1);
                render(badge, h, m, s, blink(now) ? -1 : field, field, colon, false);
                break;

            case RUN: {
                while (now - lastTick >= 1000 && remaining > 0) { remaining--; lastTick += 1000; }
                if (remaining == 0) mode = DONE;
                else if (any) mode = PAUSE;
                int dh, dm, ds; split(remaining, dh, dm, ds);
                render(badge, dh, dm, ds, -1, -1, colon, true);
                break;
            }

            case PAUSE: {
                if (btn.right.pressed) { remaining = total(); mode = SET; field = -1; }
                else if (btn.up.pressed || btn.down.pressed || btn.left.pressed) { mode = RUN; lastTick = now; }
                if ((now / 500) & 1) {
                    int dh, dm, ds; split(remaining, dh, dm, ds);
                    render(badge, dh, dm, ds, -1, -1, colon, true);
                }
                break;
            }

            case DONE:
                if (any) { remaining = total(); mode = SET; field = -1; break; }
                if ((now / 150) & 1)
                    for (int y = 0; y < 12; y++)
                        for (int x = 0; x < 20; x++) badge.setPixel(x, y, 200);
                else
                    render(badge, 0, 0, 0, -1, -1, colon, true);
                break;
        }
    }

private:
    enum Mode { SET, EDIT, RUN, PAUSE, DONE };
    Mode mode = SET;
    int h = 0, m = 5, s = 0;    // the set time (each field rolls 0..59)
    int field = -1;             // field being edited: 0=sec, 1=min, 2=hour
    int remaining = 0;          // seconds left while running or paused
    uint32_t lastTick = 0;

    int total() const { return h * 3600 + m * 60 + s; }
    static void split(int t, int& H, int& M, int& S) { H = t / 3600; M = (t / 60) % 60; S = t % 60; }
    static bool blink(uint32_t now) { return (now / 350) & 1; }

    void start(uint32_t now) {
        remaining = total();
        if (remaining == 0) return;
        mode = RUN;
        lastTick = now;
    }

    void bump(int d) {
        int& v = field == 2 ? h : field == 1 ? m : s;
        v = (v + d + 60) % 60;
    }
};

int main() {
    Badge badge;
    Timer(badge).loop();
}
