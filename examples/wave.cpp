// example by Eljakim Schrijvers, sorry for going crazy
// Two sine waves drifting across the screen and interfering. Each wave is a soft
// glowing ribbon, and where the two cross they add up and flare brighter.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "trig.hpp"

class Wave : public MainLoop {
public:
    Wave(Badge& b) : MainLoop(b) {}

    void nextFrame(Badge& badge, const FrameData&) override {
        t += 2;
        for (int x = 0; x < 20; x++) {
            int a = x * 12 + t;
            int y1 = 96 + (isin(a) * 80 >> 8);            // wave centres, in 1/16 of a row
            int y2 = 96 + (isin(a * 2 - t) * 48 >> 8);    // faster counter-wave
            for (int y = 0; y < 12; y++) {
                int c = y * 16 + 8;                       // this pixel's centre, same units
                int d1 = c - y1; if (d1 < 0) d1 = -d1;
                int d2 = c - y2; if (d2 < 0) d2 = -d2;
                int g1 = 255 - d1 * 12; if (g1 < 0) g1 = 0;   // glow falls off with distance
                int g2 = 255 - d2 * 12; if (g2 < 0) g2 = 0;
                int b = g1 + g2; if (b > 255) b = 255;        // crossings add up and flare
                if (b > 0) badge.setPixel(x, y, b);
            }
        }
    }

private:
    int t = 0;
};

int main() { Badge b; Wave(b).loop(); }
