// example by Eljakim Schrijvers, sorry for going crazy
// Two pictures: RIGHT shows world, LEFT shows hello.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"

const char* hello[12] = {
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    " # # ### #   #   ###",
    " # # #   #   #   # #",
    " ### ##  #   #   # #",
    " # # #   #   #   # #",
    " # # ### ### ### ###",
    "                    ",
    "                    ",
    "                    ",
};

const char* world[12] = {
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    " # # ### ##  #   ## ",
    " # # # # # # #   # #",
    " # # # # ##  #   # #",
    " ### # # # # #   # #",
    " # # ### # # ### ## ",
    "                    ",
    "                    ",
    "                    ",
};

class Hello2 : public MainLoop {
public:
    Hello2(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData& data) override {
        if (data.buttons.right.pressed) showWorld = true;
        if (data.buttons.left.pressed)  showWorld = false;

        const char** picture = showWorld ? world : hello;
        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 20; x++)
                if (picture[y][x] == '#')
                    badge.setPixel(x, y);
    }

private:
    bool showWorld = false;
};

int main() {
    Badge badge;
    Hello2(badge).loop();
}
