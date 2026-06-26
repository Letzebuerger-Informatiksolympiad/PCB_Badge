// example by Eljakim Schrijvers, sorry for going crazy
// A rotating spiral that also breathes from dim to bright.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"
#include "trig.hpp"

class Spiral2 : public MainLoop {
public:
    Spiral2(Badge& b) : MainLoop(b) {}

    void nextFrame(Badge& badge, const FrameData&) override {
        t += 2;
        KimsPower::setBrightness((uint8_t)(102 + (isin(t >> 2) * 94 >> 8)));

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
    Badge badge;
    Spiral2(badge).loop();
}
