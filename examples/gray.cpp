// example by Eljakim Schrijvers, sorry for going crazy
#include "sdk/Badge.hpp"

// Grayscale test: a left-to-right brightness ramp. Column 0 is dim, column 19
// is full brightness. If per-LED brightness works you see a smooth gradient
// instead of one solid block. The badge refreshes itself in the background, so we
// just draw the ramp and publish it.
int main() {
    Badge badge;
    while (true) {
        badge.clearPixels();
        for (int x = 0; x < 20; x++) {
            unsigned brightness = 16 + x * (255 - 16) / 19;   // 16 .. 255
            for (int y = 0; y < 12; y++)
                badge.setPixel(x, y, brightness);
        }
        badge.updatePixels();
    }
}
