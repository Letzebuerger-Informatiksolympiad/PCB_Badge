// example by Eljakim Schrijvers, sorry for going crazy
// Press any button to roll two dice.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include <cstdlib>

// Each face is a 3x3 grid of pip positions, packed into 9 bits (bit = row*3 + col).
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
            srand(t);                 // the moment you press decides the roll
            rolling = 2 * 92;         // tumble for about 2 seconds
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
        // Edges from 1 to 7 leave the four corners dark, so the die looks rounded.
        // The outline is dim so the bright pips stand out.
        for (int i = 1; i < 8; i++) {
            badge.setPixel(dx + i, TOP, 80);
            badge.setPixel(dx + i, TOP + 8, 80);
            badge.setPixel(dx, TOP + i, 80);
            badge.setPixel(dx + 8, TOP + i, 80);
        }
        for (int cell = 0; cell < 9; cell++)
            if (FACE[number] & (1 << cell))
                badge.setPixel(dx + 2 + (cell % 3) * 2, TOP + 2 + (cell / 3) * 2, 255);
    }
};

int main() {
    Badge badge;
    Dice(badge).loop();
}
