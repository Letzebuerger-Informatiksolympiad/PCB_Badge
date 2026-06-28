// example by Eljakim Schrijvers, sorry for going crazy
// A heart that beats.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include "trig.hpp"

const char* BIG[9] = {
    " ###   ### ",
    "###########",
    "###########",
    "###########",
    " ######### ",
    "  #######  ",
    "   #####   ",
    "    ###    ",
    "     #     ",
};
const char* SMALL[6] = {
    " ## ## ",
    "#######",
    "#######",
    " ##### ",
    "  ###  ",
    "   #   ",
};

class Heart : public MainLoop {
public:
    Heart(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData&) override {
        // The outer heart fades up from nothing and back, then rests; the core
        // stays lit the whole time.
        int s = isin(t++ * 2);
        int outer = s > 0 ? (s > 255 ? 255 : s) : 0;

        draw(badge, BIG, 9, 11, 4, 1, outer);     // breathing outer heart
        draw(badge, SMALL, 6, 7, 6, 3, 255);      // always-bright core, on top
    }

private:
    int t = 0;

    void draw(Badge& badge, const char* const* art, int h, int w, int ox, int oy, int bright) {
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++)
                if (art[y][x] == '#') badge.setPixel(ox + x, oy + y, bright);
    }
};

int main() {
    Badge badge;
    Heart(badge).loop();
}
