// example by Eljakim Schrijvers, sorry for going crazy
// Falling rain drops with little tails.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"
#include <cstdlib>

struct Drop { int x, y, len, spd, t; };

class Rain : public MainLoop {
public:
    Rain(Badge& b) : MainLoop(b) { srand(9); for (auto& d : drops) spawn(d); }

    void nextFrame(Badge& badge, const FrameData&) override {
        for (auto& d : drops) {
            if (--d.t <= 0) { d.t = d.spd; if (++d.y - d.len > 12) spawn(d); }
            for (int i = 0; i < d.len; i++) {
                int y = d.y - i;
                if (y >= 0 && y < 12) badge.setPixel(d.x, y);
            }
        }
    }

private:
    Drop drops[14];
    void spawn(Drop& d) {
        d.x = rand() % 20; d.y = -(rand() % 12);
        d.len = 2 + rand() % 4; d.spd = 3 + rand() % 4; d.t = d.spd;
    }
};

int main() { Badge b; Rain(b).loop(); }
