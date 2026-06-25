// example by Eljakim Schrijvers, sorry for going crazy
// the SDK that this was built on was developed by Luca Courte
// A wavy, dithered plasma effect.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "trig.hpp"

class Plasma : public MainLoop {
public:
    Plasma(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData& data) override {
        t += 2;
        for (int y = 0; y < 12; y++) {
            for (int x = 0; x < 20; x++) {
                int v = isin(x * 16 + t) + isin(y * 24 - t) + isin((x + y) * 12 + t / 2);
                int threshold = (dither[y % 4][x % 4] - 8) * 90;
                if (v > threshold) badge.setPixel(x, y);
            }
        }
    }

private:
    int t = 0;
    static const int dither[4][4];
};

const int Plasma::dither[4][4] = {
    { 0,  8,  2, 10},
    {12,  4, 14,  6},
    { 3, 11,  1,  9},
    {15,  7, 13,  5},
};

int main() {
    Badge badge;
    Plasma(badge).loop();
}
