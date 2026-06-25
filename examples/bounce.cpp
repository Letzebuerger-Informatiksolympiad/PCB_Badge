// example by Eljakim Schrijvers, sorry for going crazy
// the SDK that this was built on was developed by Luca Courte
// Pixels bouncing off the walls.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include <cstdlib>

struct Ball { int x, y, vx, vy; };

class Bounce : public MainLoop {
public:
    Bounce(Badge& b) : MainLoop(b) {
        srand(11);
        for (auto& bb : balls) {
            bb.x = (rand() % 20) << 4; bb.y = (rand() % 12) << 4;
            bb.vx = (rand() % 2 ? 1 : -1) * (5 + rand() % 6);
            bb.vy = (rand() % 2 ? 1 : -1) * (5 + rand() % 6);
        }
    }

    void nextFrame(Badge& badge, const FrameData&) override {
        for (auto& bb : balls) {
            bb.x += bb.vx; bb.y += bb.vy;
            if (bb.x < 0)        { bb.x = 0;        bb.vx = -bb.vx; }
            if (bb.x > (19 << 4)) { bb.x = 19 << 4; bb.vx = -bb.vx; }
            if (bb.y < 0)        { bb.y = 0;        bb.vy = -bb.vy; }
            if (bb.y > (11 << 4)) { bb.y = 11 << 4; bb.vy = -bb.vy; }
            badge.setPixel(bb.x >> 4, bb.y >> 4);
        }
    }

private:
    Ball balls[5];
};

int main() { Badge b; Bounce(b).loop(); }
