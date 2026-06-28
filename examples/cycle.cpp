// example by Eljakim Schrijvers, sorry for going crazy
// A stick figure cycling with both hands in the air.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include "trig.hpp"

class Cycle : public MainLoop {
public:
    Cycle(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData&) override {
        if (++sub >= 2) { sub = 0; t++; }
        int angle = t * 16;

        for (int x = 0; x < 20; x++)               // dim scrolling ground
            if (((x + t) % 4) == 0) badge.setPixel(x, 11, 70);

        wheel(badge, 5, 9, angle);                 // rear wheel
        wheel(badge, 13, 9, angle);                // front wheel
        line(badge, 5, 9, 9, 5, 150);              // frame to the seat
        line(badge, 13, 9, 9, 5, 150);

        int px = 9 + (icos(angle) * 2 >> 8);        // pedal goes round
        int py = 8 + (isin(angle) * 2 >> 8);
        line(badge, 9, 5, px, py, 150);            // leg down to the pedal

        badge.setPixel(9, 4, 255);                  // body, bright against the dim bike
        badge.setPixel(9, 3, 255);
        badge.setPixel(9, 2, 255);                  // head
        badge.setPixel(8, 2, 255); badge.setPixel(10, 2, 255);   // arms out...
        badge.setPixel(8, 1, 255); badge.setPixel(10, 1, 255);   // ...and up!
    }

private:
    int t = 0, sub = 0;

    void wheel(Badge& badge, int cx, int cy, int angle) {
        for (int a = 0; a < 256; a += 32)
            badge.setPixel(cx + (icos(a) * 2 >> 8), cy + (isin(a) * 2 >> 8), 150);
        badge.setPixel(cx, cy, 150);
        badge.setPixel(cx + (icos(angle) * 2 >> 8), cy + (isin(angle) * 2 >> 8), 220);  // bright spoke
    }

    void line(Badge& badge, int x0, int y0, int x1, int y1, int bright) {
        int dx = x1 > x0 ? x1 - x0 : x0 - x1;
        int dy = -(y1 > y0 ? y1 - y0 : y0 - y1);
        int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1, err = dx + dy;
        for (;;) {
            badge.setPixel(x0, y0, bright);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }
};

int main() {
    Badge badge;
    Cycle(badge).loop();
}
