// example by Eljakim Schrijvers, sorry for going crazy
// A wavy plasma effect in grayscale.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "trig.hpp"

// We add a few sine waves together for every pixel and use the sum as the pixel's
// brightness. It used to dither black-and-white to fake shading on a 1-bit screen;
// the badge now does real grayscale, so we just set the brightness straight from
// the waves and get a smooth, glowing plasma.

class Plasma : public MainLoop {
public:
    Plasma(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData& data) override {
        t += 2;
        for (int y = 0; y < 12; y++) {
            for (int x = 0; x < 20; x++) {
                // Three sine waves, each in -256..256, so v is in -768..768.
                int v = isin(x * 16 + t) + isin(y * 24 - t) + isin((x + y) * 12 + t / 2);
                // Map -768..768 to a 0..255 brightness.
                int brightness = (v + 768) * 255 / 1536;
                badge.setPixel(x, y, brightness);
            }
        }
    }

private:
    int t = 0;
};

int main() {
    Badge badge;
    Plasma(badge).loop();
}
