// example by Eljakim Schrijvers, sorry for going crazy
// The word WEOI breathing from dim to bright with the brightness control.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"
#include "trig.hpp"

const char* picture[12] = {

    "  # # ### ### ###   ",
    "  # # #   # #  #    ",
    "  # # ### # #  #    ",
    "  ### #   # #  #    ",
    "  # # ### ### ###   ",
    "                    ",
    "  ### ### ### ###   ",
    "    # # #   # #     ",
    "  ### # # ### ###   ",
    "  #   # # #   # #   ",
    "  ### ### ### ###   ",
    "                    ",
};

class Dimmer : public MainLoop {
public:
    Dimmer(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData&) override {
        t += 2;
        int level = 132 + (isin(t) * 124 >> 8);
        if (level > 255) level = 255;
        KimsPower::setBrightness((uint8_t)level);

        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 20; x++)
                if (picture[y][x] == '#')
                    badge.setPixel(x, y);
    }

private:
    int t = 0;
};

int main() {
    Badge badge;
    Dimmer(badge).loop();
}
