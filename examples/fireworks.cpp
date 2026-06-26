// example by Eljakim Schrijvers, sorry for going crazy
// the SDK that this was built on was developed by Luca Courte
// Rockets that rise and burst into rings of sparks.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"
#include "trig.hpp"
#include <cstdlib>

struct P { int x, y, vx, vy, life; bool rocket, active; };

class Fireworks : public MainLoop {
public:
    Fireworks(Badge& b) : MainLoop(b) { srand(1); }

    void nextFrame(Badge& badge, const FrameData&) override {
        if (--timer <= 0) { launch(); timer = 10 + rand() % 20; }
        for (auto& p : ps) {
            if (!p.active) continue;
            p.x += p.vx; p.y += p.vy; p.vy += 1;
            if (p.rocket) {

                if (p.vy >= 0 || (p.y >> 4) <= p.life) { explode(p); continue; }
            } else if (--p.life <= 0) { p.active = false; continue; }
            int px = p.x >> 4, py = p.y >> 4;
            if (py >= 12) { if (!p.rocket) p.active = false; continue; }
            if (px < 0 || px >= 20 || py < 0) continue;
            if (p.rocket || p.life > 5 || (p.life & 1))
                badge.setPixel(px, py);
        }
    }

private:
    P ps[90] = {};
    int timer = 1;

    P* spawn() { for (auto& p : ps) if (!p.active) return &p; return nullptr; }

    void launch() {
        P* p = spawn(); if (!p) return;
        *p = {}; p->active = true; p->rocket = true;
        p->x = (3 + rand() % 14) << 4; p->y = 11 << 4;
        p->vx = (rand() % 3 - 1) * 4; p->vy = -(16 + rand() % 6);
        p->life = 1 + rand() % 5;
    }

    void explode(P& r) {
        r.active = false;
        int cx = r.x, cy = r.y, n = 18 + rand() % 10, spd = 10 + rand() % 12;
        for (int i = 0; i < n; i++) {
            P* p = spawn(); if (!p) break;
            int a = i * 256 / n + rand() % 8;
            *p = {}; p->active = true; p->x = cx; p->y = cy;
            p->vx = icos(a) * spd >> 8; p->vy = isin(a) * spd >> 8;
            p->life = 11 + rand() % 9;
        }
    }
};

int main() { Badge b; Fireworks(b).loop(); }
