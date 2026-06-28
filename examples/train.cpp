// example by Eljakim Schrijvers, sorry for going crazy
// Parallax scenery from a train window. Trees and electricity poles slide past
// right-to-left, and how far away a thing is sets both its speed and its
// brightness: distant trees drift by dim and slow, a near pole whips past bright
// and fast. Nothing to press, just watch the world go by.
#include "sdk/Badge.hpp"
#include <pico/stdlib.h>
#include <cstdint>
#include <cstdlib>

static const int W = 20, HORIZON = 9;

// All positions are in 1/16 of a pixel; the per-layer step is the parallax speed.
// Computed x can go negative or past the edge; setPixel ignores off-screen pixels,
// so objects clip themselves as they slide in and out.
static const int FAR = 2, MID = 6, NEAR = 16, GROUND = 22;

int main() {
    Badge badge;
    badge.setFrameDelay(33);          // ~30 fps, calm and low-power
    srand(2);

    int farx[5], farv[5], midx[4], midv[4], postx[2];
    for (int i = 0; i < 5; i++) { farx[i] = (i * 5 + 2) * 16;  farv[i] = rand() % 2; }
    for (int i = 0; i < 4; i++) { midx[i] = (i * 7 + 4) * 16;  midv[i] = rand() % 3; }
    for (int i = 0; i < 2; i++)   postx[i] = (i * 45 + 26) * 16;
    int ground = 0;

    while (true) {
        badge.clearPixels();

        // A faint, steady overhead contact wire near the top.
        for (int x = 0; x < W; x++) badge.setPixel(x, 1, 36);

        // Ground band (dim), with bright dashes racing past for a sense of speed.
        for (int x = 0; x < W; x++) {
            badge.setPixel(x, 9, 48);
            badge.setPixel(x, 10, 40);
            badge.setPixel(x, 11, 34);
        }
        ground = (ground + GROUND) & (64 - 1);
        for (int x = 0; x < W; x++)
            if (((x + (ground >> 4)) & 3) == 0) badge.setPixel(x, 11, 100);

        // Far trees: tiny dim blobs just above the horizon, drifting slowly.
        for (int i = 0; i < 5; i++) {
            farx[i] -= FAR;
            if (farx[i] < -3 * 16) { farx[i] = (W + rand() % 8) * 16; farv[i] = rand() % 2; }
            int cx = farx[i] >> 4, top = 6 + farv[i];
            badge.setPixel(cx, top, 58);
            badge.setPixel(cx, top + 1, 42);
        }

        // Mid trees: a brighter canopy on a short trunk, medium speed.
        for (int i = 0; i < 4; i++) {
            midx[i] -= MID;
            if (midx[i] < -3 * 16) { midx[i] = (W + 4 + rand() % 10) * 16; midv[i] = rand() % 3; }
            int cx = midx[i] >> 4, cy = 4 + midv[i];
            for (int yy = cy; yy <= cy + 1; yy++)
                for (int xx = cx - 1; xx <= cx + 1; xx++)
                    badge.setPixel(xx, yy, 120);
            badge.setPixel(cx, cy + 2, 120);
            for (int yy = cy + 3; yy < HORIZON; yy++) badge.setPixel(cx, yy, 80);   // trunk
        }

        // Electricity poles: tall, bright, fast, and far apart, so one whips past
        // every few seconds. A crossbar near the top makes it read as a pylon.
        for (int i = 0; i < 2; i++) {
            postx[i] -= NEAR;
            if (postx[i] < -2 * 16) postx[i] = (W + 25 + rand() % 40) * 16;
            int cx = postx[i] >> 4;
            for (int yy = 2; yy <= HORIZON; yy++) badge.setPixel(cx, yy, 255);      // pole
            badge.setPixel(cx - 1, 2, 255); badge.setPixel(cx + 1, 2, 255);         // crossbar
            badge.setPixel(cx - 1, 3, 200); badge.setPixel(cx + 1, 3, 200);
        }

        badge.updatePixels();
    }
}
