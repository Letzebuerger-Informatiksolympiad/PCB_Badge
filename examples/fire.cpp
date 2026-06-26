// example by Eljakim Schrijvers, sorry for going crazy
// A flickering campfire.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"
#include <cstdlib>

class Fire : public MainLoop {
public:
    Fire(Badge& badge) : MainLoop(badge) { srand(7); }

    void nextFrame(Badge& badge, const FrameData&) override {

        for (int x = 0; x < 20; x++) {
            int edge = x < 10 ? x : 19 - x;
            int hot = edge * 5;
            if (hot > 40) hot = 40;
            heat[11][x] = hot / 2 + rand() % (hot / 2 + 10);
        }

        for (int y = 10; y >= 0; y--) {
            for (int x = 0; x < 20; x++) {
                int l = x > 0 ? x - 1 : 0, r = x < 19 ? x + 1 : 19;
                int v = (heat[y + 1][x] * 2 + heat[y + 1][l] + heat[y + 1][r]) / 4 - (rand() % 6);
                heat[y][x] = v < 0 ? 0 : v;
            }
        }

        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 20; x++)
                if (heat[y][x] > 7) badge.setPixel(x, y);
    }

private:
    int heat[12][20] = {};
};

int main() {
    Badge badge;
    Fire(badge).loop();
}
