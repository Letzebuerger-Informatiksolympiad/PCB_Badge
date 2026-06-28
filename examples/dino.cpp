// example by Eljakim Schrijvers, sorry for going crazy
// A Chrome-style endless runner. You stand on the left and the world rushes at
// you. Up jumps, Down crouches. Bright blocks are the danger: jump the ones on the
// ground, duck under the ones floating at head height. Dim cacti drift past in the
// background (just scenery, they can't hurt you). Hit a bright block and it shows
// your score; press Up or Down to run again.
#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "digits3x5.hpp"
#include <cstdlib>

struct Obj { int x; bool high; bool active; };   // x in 1/16 px

class Dino : public MainLoop {
public:
    Dino(Badge& badge) : MainLoop(badge) { badge.setFrameDelay(30); srand(9); reset(); }

    void nextFrame(Badge& badge, const FrameData& data) override {
        if (over) {
            if (data.buttons.up.pressed || data.buttons.down.pressed) reset();
            drawNumber(badge, (20 - numberWidth(score)) / 2, 4, score);
            return;
        }

        bool onGround = (jy == 0);
        crouching = onGround && data.buttons.down.down;
        if (data.buttons.up.pressed && onGround && !crouching) vy = JUMP;

        jy += vy; vy -= GRAV;                       // jump arc
        if (jy <= 0) { jy = 0; vy = 0; }

        if (--blockTimer <= 0)  { spawn(blocks, rand() % 2 == 0); blockTimer  = 26 + rand() % 26; }
        if (--cactusTimer <= 0) { spawn(cacti, false);           cactusTimer = 28 + rand() % 36; }
        for (auto& b : blocks) if (b.active) { b.x -= BLOCK_SPD;  if (b.x < -48) b.active = false; }
        for (auto& c : cacti)  if (c.active) { c.x -= CACTUS_SPD; if (c.x < -48) c.active = false; }
        groundPhase += BLOCK_SPD;
        score++;

        // Collision: the dino box vs each bright block's box.
        int f = GROUND - (jy >> 4);                 // feet row
        int dx0 = 2, dx1 = crouching ? 5 : 4;
        int dy0 = crouching ? 8 : f - 3, dy1 = crouching ? 9 : f;
        for (auto& b : blocks) if (b.active) {
            int bx = b.x >> 4;
            int by0 = b.high ? 4 : 7, by1 = b.high ? 6 : 9;
            if (dx0 <= bx + 1 && bx <= dx1 && dy0 <= by1 && by0 <= dy1) over = true;
        }

        draw(badge, f);
    }

private:
    static const int GROUND = 9, JUMP = 18, GRAV = 2, BLOCK_SPD = 9, CACTUS_SPD = 4;
    Obj blocks[6], cacti[6];
    int vy, jy, blockTimer, cactusTimer, groundPhase, score;
    bool crouching, over;

    void reset() {
        for (auto& b : blocks) b.active = false;
        for (auto& c : cacti)  c.active = false;
        vy = jy = groundPhase = score = 0;
        blockTimer = 22; cactusTimer = 8; crouching = over = false;
    }

    void spawn(Obj* pool, bool high) {
        for (int i = 0; i < 6; i++)
            if (!pool[i].active) { pool[i] = { 20 * 16, high, true }; return; }
    }

    void draw(Badge& badge, int f) {
        // Ground line with scrolling dashes (the world rushing past).
        for (int x = 0; x < 20; x++) badge.setPixel(x, 10, 45);
        for (int x = 0; x < 20; x++)
            if (((x + (groundPhase >> 4)) & 3) == 0) badge.setPixel(x, 10, 90);
        for (int x = 0; x < 20; x++) badge.setPixel(x, 11, 28);

        // Background cacti, dimly lit.
        for (auto& c : cacti) if (c.active) {
            int cx = c.x >> 4;
            badge.setPixel(cx, 7, 80); badge.setPixel(cx, 8, 80); badge.setPixel(cx, 9, 80);
            badge.setPixel(cx - 1, 8, 68); badge.setPixel(cx + 1, 7, 68);
        }

        // Foreground blocks, brightly lit.
        for (auto& b : blocks) if (b.active) {
            int bx = b.x >> 4, by0 = b.high ? 4 : 7, by1 = b.high ? 6 : 9;
            for (int y = by0; y <= by1; y++) {
                badge.setPixel(bx, y, 255);
                badge.setPixel(bx + 1, y, 255);
            }
        }

        // The dino.
        const int B = 220;
        if (crouching) {
            badge.setPixel(2, 8, B); badge.setPixel(3, 8, B); badge.setPixel(4, 8, B); badge.setPixel(5, 8, B);
            badge.setPixel(2, 9, B); badge.setPixel(4, 9, B);
        } else {
            badge.setPixel(3, f - 3, B);                                          // head
            badge.setPixel(3, f - 2, B); badge.setPixel(4, f - 2, B);             // neck
            badge.setPixel(2, f - 1, B); badge.setPixel(3, f - 1, B); badge.setPixel(4, f - 1, B);
            badge.setPixel(2, f, B); badge.setPixel(4, f, B);                     // legs
        }
    }
};

int main() {
    Badge badge;
    Dino(badge).loop();
}
