// example by Eljakim Schrijvers, sorry for going crazy
// obviously I did not invent flappy bird
// Flap with any button to fly the bright bird through the gaps.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include "digits3x5.hpp"
#include <cstdlib>

class Flappy : public MainLoop {
public:
    Flappy(Badge& badge) : MainLoop(badge) { badge.setFrameDelay(40); srand(1); reset(); }

    void nextFrame(Badge& badge, const FrameData& data) override {
        bool tap = data.buttons.up.pressed || data.buttons.down.pressed ||
                   data.buttons.left.pressed || data.buttons.right.pressed;

        if (dead) {
            if (tap) reset();
            drawNumber(badge, (20 - numberWidth(score)) / 2, 4, score);
            return;
        }

        if (tap) vy = -14;                 // a gentle flap (units: 1/16 px / frame)
        vy += 2;                           // gravity
        if (vy > 22) vy = 22;              // terminal fall speed
        by += vy;
        if (by < 0) { by = 0; vy = 0; }    // bonk the ceiling, but survive
        if (by > (11 << 4)) dead = true;   // hit the floor

        for (auto& p : pipes) {
            p.x -= 5;                       // scroll left (units: 1/16 px / frame)
            if ((p.x >> 4) < BIRD_X && !p.scored) { score++; p.scored = true; }
            if (p.x < -16) respawn(p);
        }

        int by_px = by >> 4;
        for (auto& p : pipes)
            if ((p.x >> 4) == BIRD_X && (by_px < p.gap || by_px >= p.gap + GAP))
                dead = true;

        for (auto& p : pipes) {
            int col = p.x >> 4;
            if (col < 0 || col >= 20) continue;
            for (int y = 0; y < 12; y++)
                if (y < p.gap || y >= p.gap + GAP) badge.setPixel(col, y, 120);
        }
        badge.setPixel(BIRD_X, by_px, 255);
    }

private:
    static const int BIRD_X = 4;
    static const int GAP = 5;       // rows of clear space in each wall
    static const int SPACING = 16;  // pixels between successive walls
    struct Pipe { int x, gap; bool scored; };
    Pipe pipes[2];
    int by, vy, score;
    bool dead;

    void reset() {
        by = 6 << 4; vy = 0; score = 0; dead = false;
        pipes[0].x = 24 << 4; pipes[1].x = (24 + SPACING) << 4;
        for (auto& p : pipes) { p.gap = 1 + rand() % (12 - GAP - 1); p.scored = false; }
    }

    void respawn(Pipe& p) {
        int far = pipes[0].x > pipes[1].x ? pipes[0].x : pipes[1].x;
        p.x = far + (SPACING << 4);
        p.gap = 1 + rand() % (12 - GAP - 1);
        p.scored = false;
    }
};

int main() {
    Badge badge;
    Flappy(badge).loop();
}
