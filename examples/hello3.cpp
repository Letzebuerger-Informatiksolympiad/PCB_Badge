// example by Eljakim Schrijvers, sorry for going crazy
// A picture wider than the screen; scroll it with LEFT and RIGHT.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"

const char* picture[12] = {
    "                                            ",
    "                                            ",
    "                                            ",
    "                                            ",
    " # # ### #   #   ###     # # ### ##  #   ## ",
    " # # #   #   #   # #     # # # # # # #   # #",
    " ### ##  #   #   # #     # # # # ##  #   # #",
    " # # #   #   #   # #     ### # # # # #   # #",
    " # # ### ### ### ###     # # ### # # ### ## ",
    "                                            ",
    "                                            ",
    "                                            ",
};

class Hello3 : public MainLoop {
public:
    Hello3(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData& data) override {

        if (--ticks <= 0) {
            ticks = 3;
            if (data.buttons.right.down && scroll < 44 - 20) scroll++;
            if (data.buttons.left.down  && scroll > 0)             scroll--;
        }

        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 20; x++)
                if (picture[y][x + scroll] == '#')
                    badge.setPixel(x, y);
    }

private:
    int scroll = 0, ticks = 1;
};

int main() {
    Badge badge;
    Hello3(badge).loop();
}
