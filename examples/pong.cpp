// example by Eljakim Schrijvers, sorry for going crazy
// the SDK that this was built on was developed by Luca Courte
// Pong against the computer; you are the left paddle (UP/DOWN).

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"
#include <cstdlib>

class Pong : public MainLoop {
public:
    Pong(Badge& badge) : MainLoop(badge) { left = right = 4; serve(1); }

    void nextFrame(Badge& badge, const FrameData& data) override {
        if (--moveTick <= 0) {
            moveTick = 2;
            if (data.buttons.up.down   && left > 0)        left--;
            if (data.buttons.down.down && left < 12 - PAD) left++;

            int middle = right + PAD / 2, target = ballY >> 4;
            if (target < middle && right > 0)             right--;
            else if (target > middle && right < 12 - PAD) right++;
        }

        ballX += vx; ballY += vy;
        if (ballY < 0)         { ballY = 0;       vy = -vy; }
        if (ballY > (11 << 4)) { ballY = 11 << 4; vy = -vy; }

        int bx = ballX >> 4, by = ballY >> 4;
        if (bx <= 1  && by >= left  && by < left  + PAD && vx < 0) bounce(left);
        if (bx >= 18 && by >= right && by < right + PAD && vx > 0) bounce(right);
        if (bx < 0)  serve(-1);
        if (bx > 19) serve(1);

        for (int y = 0; y < 12; y += 2) badge.setPixel(10, y);
        for (int i = 0; i < PAD; i++) {
            badge.setPixel(0, left + i);
            badge.setPixel(19, right + i);
        }
        badge.setPixel(ballX >> 4, ballY >> 4);
    }

private:
    static const int PAD = 3;
    int left, right, ballX, ballY, vx, vy, moveTick = 2;

    void serve(int dir) {
        ballX = 10 << 4; ballY = 6 << 4;
        vx = dir * 4;
        vy = (rand() % 2 ? 3 : -3);
    }

    void bounce(int paddle) {
        int speed = vx < 0 ? -vx : vx;
        if (speed < 9) speed++;
        vx = vx < 0 ? speed : -speed;
        vy += ((ballY >> 4) - (paddle + PAD / 2)) * 5;
        if (vy > 12) vy = 12;
        if (vy < -12) vy = -12;
    }
};

int main() {
    Badge badge;
    Pong(badge).loop();
}
