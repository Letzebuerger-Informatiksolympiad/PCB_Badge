// example by Eljakim Schrijvers, sorry for going crazy
// A top-down racing game. Steer Left/Right to keep your car on the winding road
// and dodge the blocks. The road fades into the distance with grayscale: the far
// road is dim, the near road is bright. Crash and it shows how far you got; press
// any button to race again.
#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "digits3x5.hpp"
#include <cstdlib>

class Racing : public MainLoop {
public:
    Racing(Badge& badge) : MainLoop(badge) { badge.setFrameDelay(33); srand(3); reset(); }

    void nextFrame(Badge& badge, const FrameData& data) override {
        bool any = data.buttons.up.pressed || data.buttons.down.pressed ||
                   data.buttons.left.pressed || data.buttons.right.pressed;
        if (over) {
            if (any) reset();
            drawNumber(badge, (20 - numberWidth(score)) / 2, 4, score);
            return;
        }

        // Steer: one step every couple of frames while held, so it stays controllable.
        if (steerCool > 0) steerCool--;
        if (steerCool == 0) {
            if (data.buttons.left.down  && carX > 0)  { carX--; steerCool = 2; }
            if (data.buttons.right.down && carX < 19) { carX++; steerCool = 2; }
        }

        // Scroll the road toward you every `speed` frames.
        if (++scrollT >= speed) { scrollT = 0; scroll(); }

        // Crash if the car is off the road or on a block at its row.
        int l = center[CAR_ROW] - HALF, r = center[CAR_ROW] + HALF;
        if (carX < l || carX > r || obs[CAR_ROW] == carX) over = true;

        draw(badge);
    }

private:
    static const int CAR_ROW = 10;
    static const int HALF    = 4;          // road is 2*HALF+1 = 9 wide
    int center[12], obs[12], carX, scrollT, speed, score, curve, steerCool;
    bool over;

    void reset() {
        for (int y = 0; y < 12; y++) { center[y] = 10; obs[y] = -1; }
        carX = 10; scrollT = 0; speed = 4; score = 0; curve = 0; steerCool = 0; over = false;
    }

    void scroll() {
        for (int y = 11; y > 0; y--) { center[y] = center[y - 1]; obs[y] = obs[y - 1]; }
        // Wander the road gently, bouncing off the edges of the screen.
        curve += rand() % 3 - 1;
        if (curve < -1) curve = -1;
        if (curve > 1)  curve = 1;
        int nc = center[1] + curve;
        if (nc < HALF + 1)  { nc = HALF + 1;  curve = 1; }
        if (nc > 18 - HALF) { nc = 18 - HALF; curve = -1; }
        center[0] = nc;
        // Sometimes drop a block somewhere on the new far row of road.
        obs[0] = (rand() % 100 < 12) ? nc - HALF + 1 + rand() % (2 * HALF - 1) : -1;
        score++;
        if (score % 25 == 0 && speed > 2) speed--;       // creep faster over time
    }

    void draw(Badge& badge) {
        for (int y = 0; y < 12; y++) {
            int depth = 50 + y * 17;                     // far rows dim, near rows bright
            int l = center[y] - HALF, r = center[y] + HALF;
            for (int x = l; x <= r; x++) {
                if (x < 0 || x > 19) continue;
                badge.setPixel(x, y, (x == l || x == r) ? depth : depth / 3);
            }
            if (obs[y] >= 0) {
                int b = depth + 80; if (b > 255) b = 255;
                badge.setPixel(obs[y], y, b);            // blocks: brighter than the road
            }
        }
        badge.setPixel(carX, CAR_ROW, 255);              // your car
        badge.setPixel(carX, CAR_ROW + 1, 255);
    }
};

int main() {
    Badge badge;
    Racing(badge).loop();
}
