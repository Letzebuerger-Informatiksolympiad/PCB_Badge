// example by Eljakim Schrijvers, sorry for going crazy
// Show a picture you draw on the screen, pixel by pixel.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"

const char* picture[12] = {
    "                    ",
    " # # ### #   #   ###",
    " # # #   #   #   # #",
    " ### ##  #   #   # #",
    " # # #   #   #   # #",
    " # # ### ### ### ###",
    "                    ",
    " # # ### ##  #   ## ",
    " # # # # # # #   # #",
    " # # # # ##  #   # #",
    " ### # # # # #   # #",
    " # # ### # # ### ## ",
};

class Hello : public MainLoop {
public:
    Hello(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData& data) override {
        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 20; x++)
                if (picture[y][x] == '#')
                    badge.setPixel(x, y);
    }
};

int main() {
    Badge badge;
    Hello(badge).loop();
}
