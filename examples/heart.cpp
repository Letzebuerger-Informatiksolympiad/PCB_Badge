// example by Eljakim Schrijvers, sorry for going crazy
// A heart that beats.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"

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
        int beat = (t++) % 75;
        bool big = (beat < 6) || (beat >= 12 && beat < 18);
        if (big) draw(badge, BIG, 9, 11, 4, 1);
        else     draw(badge, SMALL, 6, 7, 6, 3);
    }

private:
    int t = 0;

    void draw(Badge& badge, const char* const* art, int h, int w, int ox, int oy) {
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++)
                if (art[y][x] == '#') badge.setPixel(ox + x, oy + y);
    }
};

int main() {
    Badge badge;
    Heart(badge).loop();
}
