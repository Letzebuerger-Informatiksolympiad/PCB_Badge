// example by Eljakim Schrijvers, sorry for going crazy
// the SDK that this was built on was developed by Luca Courte
// Fly through a field of stars.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"
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
            badge.setPixel(px, py);
        }
    }

private:
    Star stars[40];
    void reset(Star& s, bool anyDepth) {
        s.x = rand() % 2048 - 1024;
        s.y = rand() % 2048 - 1024;
        s.z = anyDepth ? 1 + rand() % 256 : 256;
    }
};

int main() { Badge b; Starfield(b).loop(); }
