// example by Eljakim Schrijvers, sorry for going crazy
// the SDK that this was built on was developed by Luca Courte
// Press any button to roll two dice.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include <cstdlib>

static const int FACE[7] = {0, 0b000010000, 0b100000001, 0b100010001,
                               0b101000101, 0b101010101, 0b101101101};

class Dice : public MainLoop {
public:
    Dice(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData& data) override {
        t++;
        bool pressed = data.buttons.up.pressed || data.buttons.down.pressed ||
                       data.buttons.left.pressed || data.buttons.right.pressed;
        if (pressed) {
            srand(t);
            rolling = 2 * 92;
        }

        if (rolling > 0) {
            if (rolling % 5 == 0) { left = 1 + rand() % 6; right = 1 + rand() % 6; }
            rolling--;
        }

        drawDie(badge, 0, left);
        drawDie(badge, 11, right);
    }

private:
    static const int TOP = 1;
    int left = 1, right = 1, rolling = 0, t = 0;

    void drawDie(Badge& badge, int dx, int number) {

        for (int i = 1; i < 8; i++) {
            badge.setPixel(dx + i, TOP);
            badge.setPixel(dx + i, TOP + 8);
            badge.setPixel(dx, TOP + i);
            badge.setPixel(dx + 8, TOP + i);
        }
        for (int cell = 0; cell < 9; cell++)
            if (FACE[number] & (1 << cell))
                badge.setPixel(dx + 2 + (cell % 3) * 2, TOP + 2 + (cell / 3) * 2);
    }
};

int main() {
    Badge badge;
    Dice(badge).loop();
}
