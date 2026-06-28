// example by Eljakim Schrijvers, sorry for going crazy
// Rotating spiral arms.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"
#include "trig.hpp"

class Spiral : public MainLoop {
public:
    Spiral(Badge& b) : MainLoop(b) {}

    void nextFrame(Badge& badge, const FrameData&) override {
        t += 2;
        for (int r = 0; r < 12; r++) {
            for (int arm = 0; arm < 3; arm++) {
                int a = r * 18 + t + arm * 85;
                int x = 10 + (icos(a) * r >> 8);
                int y = 6 + (isin(a) * r >> 9);
                if (x >= 0 && x < 20 && y >= 0 && y < 12) badge.setPixel(x, y);
            }
        }
    }

private:
    int t = 0;
};

int main() {
    Badge b;
    KimsPower::setBrightness(128);
    Spiral(b).loop();
}
