// example by Eljakim Schrijvers, sorry for going crazy
// The word WEOI breathing from dim to bright.

#include "sdk/Badge.hpp"
#include "sdk/KimsPower.hpp"
#include "trig.hpp"
#include <pico/stdlib.h>

const char* picture[12] = {

    "  # # ### ### ###   ",
    "  # # #   # #  #    ",
    "  # # ### # #  #    ",
    "  ### #   # #  #    ",
    "  # # ### ### ###   ",
    "                    ",
    "  ### ### ### ###   ",
    "    # # #   # #     ",
    "  ### # # ### ###   ",
    "  #   # # #   # #   ",
    "  ### ### ### ###   ",
    "                    ",
};

int main() {
    Badge badge;

    // Draw the word once. The badge refreshes the display in the background, so the
    // word stays on the screen on its own; we never touch the pixels again.
    for (int y = 0; y < 12; y++)
        for (int x = 0; x < 20; x++)
            if (picture[y][x] == '#')
                badge.setPixel(x, y);
    badge.updatePixels();

    // Breathe by nudging the global brightness, which is a hardware PWM on the
    // panel's output-enable pin. So all the CPU does now is write one register and
    // sleep; the dimming and the refresh both run in hardware while it idles.
    int t = 0;
    while (true) {
        t += 2;
        int level = 132 + (isin(t) * 124 >> 8);
        if (level > 255) level = 255;
        KimsPower::setBrightness((uint8_t)level);
        sleep_ms(26);
    }
}
