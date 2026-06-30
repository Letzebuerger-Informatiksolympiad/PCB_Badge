// example by Eljakim Schrijvers, sorry for going crazy
// A heart that beats, with the glow rolling outward from the core.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include "trig.hpp"

const char* BIG[9] = {
    " ###   ### ",
    "###########",
    "###########",
    "###########",
    " ######### ",
    "  #######  ",
    "   #####   ",
    "    ###    ",
    "     #     ",
};
const char* SMALL[6] = {
    " ## ## ",
    "#######",
    "#######",
    " ##### ",
    "  ###  ",
    "   #   ",
};

class Heart : public MainLoop {
public:
    Heart(Badge& badge) : MainLoop(badge) {
        // Erosion depth of the big heart: 0 = background, 1 = outermost shell of
        // pixels, larger = deeper toward the centre. Each pixel's depth is its
        // Chebyshev distance to the nearest background cell, which we solve by
        // relaxing depth = 1 + min(8-neighbour depths) to a fixpoint. The grid
        // is tiny, so H+W passes are far more than enough to converge.
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
                depth[y][x] = (BIG[y][x] == '#') ? 1 : 0;
        for (int pass = 0; pass < H + W; pass++)
            for (int y = 0; y < H; y++)
                for (int x = 0; x < W; x++) {
                    if (BIG[y][x] != '#') continue;
                    int lo = 99;
                    for (int dy = -1; dy <= 1; dy++)
                        for (int dx = -1; dx <= 1; dx++) {
                            if (!dx && !dy) continue;
                            int ny = y + dy, nx = x + dx;
                            bool out = ny < 0 || ny >= H || nx < 0 || nx >= W;
                            int nd = out ? 0 : depth[ny][nx];
                            if (nd < lo) lo = nd;
                        }
                    depth[y][x] = lo + 1;
                }
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++)
                if (depth[y][x] > maxDepth) maxDepth = depth[y][x];
    }

    void nextFrame(Badge& badge, const FrameData&) override {
        // One shared breath per cycle: g rises, falls, then rests. t++ * 2 keeps
        // the original speed. Each ring gates that breath behind a brightness
        // threshold that grows from 0 at the core to TMAX at the rim. On the way
        // up nothing switches off, so the glow fills from the inside out (an
        // outer ring only starts once the ring inside it is ~50% lit); on the
        // way down the outer rings drop first, so it recedes from the outside in
        // and the inner rings stay lit longest. No travelling gap.
        int phase = t++ * 2;
        int g = isin(phase);
        if (g < 0) g = 0; else if (g > 255) g = 255;
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++) {
                if (BIG[y][x] != '#') continue;
                int thr = maxDepth > 1 ? (maxDepth - depth[y][x]) * TMAX / (maxDepth - 1) : 0;
                int v = g - thr;
                int b = v <= 0 ? 0 : v * 255 / (255 - thr);   // rescale so the ring still reaches full at the peak
                if (b > 255) b = 255;
                badge.setPixel(OX + x, OY + y, b);
            }

        // Always-bright core, drawn on top.
        for (int y = 0; y < 6; y++)
            for (int x = 0; x < 7; x++)
                if (SMALL[y][x] == '#') badge.setPixel(6 + x, 3 + y, 255);
    }

private:
    static const int H = 9, W = 11, OX = 4, OY = 1, TMAX = 127;
    int depth[H][W] = {};
    int maxDepth = 0;
    int t = 0;
};

int main() {
    Badge badge;
    Heart(badge).loop();
}
