// example by Eljakim Schrijvers, sorry for going crazy
// obviously I did not come up with the idea of snake
// Snake you steer with the buttons; shows your score when you lose.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "digits3x5.hpp"
#include <cstdlib>

class Snake : public MainLoop {
public:
    Snake(Badge& badge) : MainLoop(badge) { srand(5); reset(); }

    void nextFrame(Badge& badge, const FrameData& data) override {
        bool anyButton = data.buttons.up.pressed || data.buttons.down.pressed ||
                         data.buttons.left.pressed || data.buttons.right.pressed;
        if (over) {
            if (anyButton) reset();
            drawNumber(badge, (20 - numberWidth(eaten)) / 2, 4, eaten);
            return;
        }

        if (data.buttons.up.pressed    && dy == 0) { dx = 0;  dy = -1; }
        if (data.buttons.down.pressed  && dy == 0) { dx = 0;  dy = 1; }
        if (data.buttons.left.pressed  && dx == 0) { dx = -1; dy = 0; }
        if (data.buttons.right.pressed && dx == 0) { dx = 1;  dy = 0; }
        if (--timer <= 0) { timer = 14; move(); }

        // Draw the body as a glowing comet: a bright head that fades down to a
        // dim tail, so you can see which way the snake is heading.
        for (int i = 0; i < len; i++) {
            int b = 255 - i * 18;
            if (b < 50) b = 50;
            badge.setPixel(sx[i], sy[i], b);
        }
        badge.setPixel(fx, fy);            // the food, full brightness
    }

private:
    static const int MAX = 240;
    int sx[MAX], sy[MAX], len, dx, dy, fx, fy, timer, eaten;
    bool over;

    void reset() {
        len = 4;
        for (int i = 0; i < len; i++) { sx[i] = 8 - i; sy[i] = 6; }
        dx = 1; dy = 0; timer = 14; eaten = 0; over = false; food();
    }
    void food() { fx = rand() % 20; fy = rand() % 12; }

    void move() {
        int nx = (sx[0] + dx + 20) % 20, ny = (sy[0] + dy + 12) % 12;
        for (int i = 0; i < len; i++)
            if (sx[i] == nx && sy[i] == ny) { over = true; return; }
        for (int i = len; i > 0; i--) { sx[i] = sx[i - 1]; sy[i] = sy[i - 1]; }
        sx[0] = nx; sy[0] = ny;
        if (nx == fx && ny == fy) { if (len < MAX) len++; eaten++; food(); }
    }
};

int main() {
    Badge badge;
    Snake(badge).loop();
}
