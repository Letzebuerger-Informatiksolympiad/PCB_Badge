// example by Eljakim Schrijvers, sorry for going crazy
// A calm black-and-white campfire in grayscale.

// Every pixel holds a "heat" value (0..255) that is also its brightness, so the
// fire is drawn in real grayscale, not just on/off. Each step the bottom row
// glows with fresh embers and the heat above it drifts up and cools, so flames
// rise and fade. It reads the previous frame, so heat climbs about one row per
// step and the fire changes slowly. It runs at a low, fuel-sipping frame rate
// while the badge refreshes the screen in the background.
#include "sdk/Badge.hpp"
#include <pico/stdlib.h>
#include <cstdint>
#include <cstdlib>

int main() {
    Badge badge;
    badge.setFrameDelay(60);          // slow and calm (~16 fps); the CPU idles between frames
    srand(7);

    static uint8_t heat[12][20] = {};
    while (true) {
        // Embers along the bottom: hottest in the middle, flickering. We ease toward
        // a new value instead of jumping to it, so the base glows rather than buzzes.
        for (int x = 0; x < 20; x++) {
            int edge = x < 10 ? x : 19 - x;          // 0 at the sides, 9 in the middle
            int target = 150 + edge * 11 + rand() % 40;
            if (target > 255) target = 255;
            heat[11][x] = (heat[11][x] + target) / 2;
        }

        // Each pixel takes heat from the three pixels just below it, then cools.
        // Reading the previous frame means heat rises about one row per step, so the
        // flames climb gently instead of snapping to full height.
        uint8_t next[12][20];
        for (int x = 0; x < 20; x++) next[11][x] = heat[11][x];
        for (int y = 10; y >= 0; y--) {
            for (int x = 0; x < 20; x++) {
                int l = x > 0 ? x - 1 : 0, r = x < 19 ? x + 1 : 19;
                int v = (heat[y + 1][x] * 2 + heat[y + 1][l] + heat[y + 1][r]) / 4;
                v -= 16 + rand() % 16;               // cool as it rises
                next[y][x] = v < 0 ? 0 : v;
            }
        }
        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 20; x++)
                heat[y][x] = next[y][x];

        // The heat is the brightness: white-hot at the base, fading to black on top.
        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 20; x++)
                badge.setPixel(x, y, heat[y][x]);

        badge.updatePixels();
    }
}
