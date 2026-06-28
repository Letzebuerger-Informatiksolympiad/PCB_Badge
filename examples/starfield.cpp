// example by Eljakim Schrijvers, sorry for going crazy
// Fly through a field of stars.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include <cstdlib>

struct Star { int x, y, z; };

class Starfield : public MainLoop {
public:
    Starfield(Badge& b) : MainLoop(b) { srand(7); for (auto& s : stars) reset(s, true); }

    void nextFrame(Badge& badge, const FrameData&) override {
        for (auto& s : stars) {
            s.z -= 3;
            if (s.z <= 0) { reset(s, false); continue; }
            int px = 10 + s.x / s.z;
            int py = 6 + s.y / s.z;
            if (px < 0 || px >= 20 || py < 0 || py >= 12) { reset(s, false); continue; }
            // Stars are born dark near the center and brighten as they streak
            // out toward the edges, so they seem to rush past you.
            int dx = px - 10, dy = py - 6;
            int b = 35 + (dx * dx + dy * dy) * 220 / 80;    // dark center, ramps up quickly
            if (b > 255) b = 255;
            badge.setPixel(px, py, b);
        }
    }

private:
    Star stars[40];
    void reset(Star& s, bool anyDepth) {
        s.x = rand() % 2048 - 1024;
        s.y = rand() % 2048 - 1024;
        s.z = anyDepth ? 1 + rand() % 256 : 256;   // new stars start far away
    }
};

int main() { Badge b; Starfield(b).loop(); }
