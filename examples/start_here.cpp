// example by Eljakim Schrijvers, sorry for going crazy
#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it

// This is your starting point. Make it your own!
//
// The screen is 20 wide and 12 tall. badge.setPixel(x, y) turns one pixel on.
// nextFrame() runs once per frame; the screen is cleared before each one.
//
// Right now it lights every pixel in turn, one after another. Change the code
// below and press Run to see what happens.
class StartHere : public MainLoop {
public:
    StartHere(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData& data) override {
        badge.setPixel(x, y);          // light the pixel we are on

        if (++wait >= 4) {             // wait a few frames, then step to the next
            wait = 0;
            if (++x >= 20) {           // past the right edge? go to the next row
                x = 0;
                if (++y >= 12) y = 0;  // past the bottom? start over at the top
            }
        }
    }

private:
    int x = 0, y = 0, wait = 0;
};

int main() {
    Badge badge;
    StartHere(badge).loop();
}
