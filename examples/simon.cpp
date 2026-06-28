// example by Eljakim Schrijvers, sorry for going crazy
// the classic memory game, drawn as four triangles meeting in the middle.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "digits3x5.hpp"
#include <cstdlib>

// 0 = top (Up), 1 = bottom (Down), 2 = left (Left), 3 = right (Right).
static const uint8_t REST[4] = { 45, 85, 130, 175 };   // distinct idle grays

class Simon : public MainLoop {
public:
    Simon(Badge& badge) : MainLoop(badge) { reset(); }

    void nextFrame(Badge& badge, const FrameData& data) override {
        t++;

        if (phase == OVER) {
            bool any = data.buttons.up.pressed || data.buttons.down.pressed ||
                       data.buttons.left.pressed || data.buttons.right.pressed;
            int blink = (t / 8) % 2 ? 200 : 30;          // blink the whole screen
            paint(badge, blink, blink, blink, blink);
            drawNumber(badge, (20 - numberWidth(len - 1)) / 2, 4, len - 1);
            if (any) reset();
            return;
        }

        int bright[4] = { REST[0], REST[1], REST[2], REST[3] };

        if (phase == PAUSE) {
            // Breather after a correct round before the next pattern plays.
            paint(badge, bright[0], bright[1], bright[2], bright[3]);
            drawNumber(badge, (20 - numberWidth(len)) / 2, 4, len);
            if (t >= pauseUntil) startShow();
            return;
        }

        if (phase == SHOW) {
            // Light each step for a beat, with a dark gap between them.
            int beat = t - showStart;
            if (beat < ON) bright[seq[showIdx]] = 255;
            else if (beat >= ON + OFF) {
                showStart = t;
                if (++showIdx >= len) { phase = INPUT; inputIdx = 0; }
            }
        } else if (phase == INPUT) {
            int pick = -1;
            if (data.buttons.up.pressed)    pick = 0;
            if (data.buttons.down.pressed)  pick = 1;
            if (data.buttons.left.pressed)  pick = 2;
            if (data.buttons.right.pressed) pick = 3;
            if (pick >= 0) {
                flashWhich = pick; flashUntil = t + ON;
                if (pick != seq[inputIdx]) phase = OVER;
                else if (++inputIdx >= len) grow();
            }
            if (t < flashUntil) bright[flashWhich] = 255;
        }

        paint(badge, bright[0], bright[1], bright[2], bright[3]);
        drawNumber(badge, (20 - numberWidth(len)) / 2, 4, len);
    }

private:
    enum Phase { SHOW, INPUT, PAUSE, OVER };
    static const int ON = 28, OFF = 16;   // frames a step is lit / dark
    int seq[64], len, showIdx, inputIdx, showStart, flashWhich, flashUntil, pauseUntil, t = 0;
    Phase phase;

    void reset() {
        srand(t + 1);
        len = 1; seq[0] = rand() % 4;
        startShow();
    }

    void grow() {
        if (len < 64) seq[len++] = rand() % 4;
        phase = PAUSE; pauseUntil = t + 200;   // wait ~2s before the next pattern
    }

    void startShow() { phase = SHOW; showIdx = 0; showStart = t; flashUntil = 0; }

    // Which triangle a pixel falls in: split the screen along its two diagonals.
    int triangle(int x, int y) {
        int nx = 2 * x - 19, ny = 2 * y - 11;            // offset from the center
        if (abs(ny) * 20 > abs(nx) * 12) return ny < 0 ? 0 : 1;   // top / bottom
        return nx < 0 ? 2 : 3;                            // left / right
    }

    void paint(Badge& badge, int top, int bottom, int left, int right) {
        int b[4] = { top, bottom, left, right };
        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 20; x++)
                badge.setPixel(x, y, b[triangle(x, y)]);
    }
};

int main() {
    Badge badge;
    Simon(badge).loop();
}
