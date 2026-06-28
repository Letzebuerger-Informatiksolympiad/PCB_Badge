// example by Eljakim Schrijvers, sorry for going crazy
// Conway's Game of Life; press any button to start over.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include <cstdlib>
#include <cstring>

class Life : public MainLoop {
public:
    Life(Badge& b) : MainLoop(b) { srand(3); seed(); }

    void nextFrame(Badge& badge, const FrameData& data) override {
        if (data.buttons.up.pressed || data.buttons.down.pressed ||
            data.buttons.left.pressed || data.buttons.right.pressed) seed();
        if (--timer <= 0) { timer = 22; stepLife(); }
        // The longer a cell has been alive, the brighter it glows: fresh births
        // are dim, long-lived cells are full bright.
        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 20; x++)
                if (age[y][x]) badge.setPixel(x, y, age[y][x] >= 6 ? 255 : 60 + age[y][x] * 32);
    }

private:
    bool grid[12][20] = {}, next[12][20] = {};
    int age[12][20] = {};
    int timer = 22, stale = 0;

    void seed() {
        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 20; x++) { grid[y][x] = rand() % 100 < 35; age[y][x] = grid[y][x]; }
        stale = 0;
    }

    void stepLife() {
        int live = 0, changed = 0;
        for (int y = 0; y < 12; y++) {
            for (int x = 0; x < 20; x++) {
                int n = 0;
                for (int dy = -1; dy <= 1; dy++)
                    for (int dx = -1; dx <= 1; dx++) {
                        if (!dx && !dy) continue;
                        n += grid[(y + dy + 12) % 12][(x + dx + 20) % 20];
                    }
                next[y][x] = (n == 3) || (grid[y][x] && n == 2);
                if (next[y][x]) { live++; if (age[y][x] < 99) age[y][x]++; } else age[y][x] = 0;
                if (next[y][x] != grid[y][x]) changed++;
            }
        }
        memcpy(grid, next, sizeof grid);
        if (live == 0 || changed < 2) { if (++stale > 3) seed(); } else stale = 0;
    }
};

int main() { Badge b; Life(b).loop(); }
