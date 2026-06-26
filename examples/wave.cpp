// example by Eljakim Schrijvers, sorry for going crazy
// the SDK that this was built on was developed by Luca Courte
// Two sine waves drifting across the screen.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"
#include "trig.hpp"

class Wave : public MainLoop {
public:
    Wave(Badge& b) : MainLoop(b) {}

    void nextFrame(Badge& badge, const FrameData&) override {
        t += 2;
        for (int x = 0; x < 20; x++) {
            int a = x * 12 + t;
            int y1 = 6 + (isin(a) * 5 >> 8);
            int y2 = 6 + (isin(a * 2 - t) * 3 >> 8);
            if (y1 >= 0 && y1 < 12) badge.setPixel(x, y1);
            if (y2 >= 0 && y2 < 12) badge.setPixel(x, y2);
        }
    }

private:
    int t = 0;
};

int main() { Badge b; Wave(b).loop(); }
