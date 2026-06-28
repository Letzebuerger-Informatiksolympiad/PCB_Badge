// example by Eljakim Schrijvers, sorry for going crazy
// A plasma spiral: a bright core with arms radiating light outward.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include "trig.hpp"

class Spiral3 : public MainLoop {
public:
    Spiral3(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData&) override {
        t += 5;
        for (int y = 0; y < 12; y++) {
            for (int x = 0; x < 20; x++) {
                int dx = x - 10, dy = y - 6;
                int ang = iatan2(dy, dx);                 // 0..255 around the center
                int r = isqrt(dx * dx + dy * dy);         // distance from the center

                // Spiral wave: arms twist with the angle and roll outward over time.
                int wave = isin(ang * ARMS + r * PITCH - t);
                int b = 128 + (wave >> 1);                // smooth plasma, 0..255

                int core = 255 - r * 34;                  // a core that is always lit
                if (core > b) b = core;
                if (b > 255) b = 255;
                if (b > 0) badge.setPixel(x, y, b);
            }
        }
    }

private:
    static const int ARMS = 2;    // number of spiral arms
    static const int PITCH = 22;  // how tightly the arms wind
    int t = 0;

    int isqrt(int v) {
        int r = 0;
        while ((r + 1) * (r + 1) <= v) r++;
        return r;
    }

    // Angle of (x, y) as 0..255 for a full turn (a cheap integer atan2).
    int iatan2(int y, int x) {
        if (x == 0 && y == 0) return 0;
        int ay = y < 0 ? -y : y;
        int angle = x >= 0 ? 32 - 32 * (x - ay) / (x + ay)
                           : 96 - 32 * (x + ay) / (ay - x);
        if (y < 0) angle = -angle;
        return angle & 255;
    }
};

int main() {
    Badge badge;
    Spiral3(badge).loop();
}
