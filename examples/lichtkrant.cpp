// example by Eljakim Schrijvers, sorry for going crazy
// Scrolling ticker; UP/DOWN change the message, LEFT/RIGHT the speed.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include "font5x7.hpp"
#include <cstring>

class Lichtkrant : public MainLoop {
public:
    Lichtkrant(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData& data) override {
        if (data.buttons.up.pressed)   { message = (message + 1) % COUNT;         pos = 0; }
        if (data.buttons.down.pressed) { message = (message + COUNT - 1) % COUNT; pos = 0; }
        if (data.buttons.right.pressed && delay > 1)  delay--;   // faster
        if (data.buttons.left.pressed  && delay < 24) delay++;   // slower

        const char* text = MESSAGES[message];
        int textWidth = (int)strlen(text) * (FONT_W + 1);

        // Move one pixel every `delay` frames so it is easy to read.
        if (--ticks <= 0) {
            ticks = delay;
            pos++;
            if (pos > textWidth + 20) pos = 0;
        }

        int top = (12 - FONT_H) / 2;
        for (int i = 0; text[i]; i++) {
            Glyph g = fontGlyph(text[i]);
            int letterX = 20 + i * (FONT_W + 1) - pos;
            for (int x = 0; x < FONT_W; x++) {
                int screenX = letterX + x;
                if (screenX < 0 || screenX >= 20) continue;
                // Fade letters in and out near the two edges of the screen.
                int edge = screenX < 19 - screenX ? screenX : 19 - screenX;
                int bright = edge >= 4 ? 255 : 70 + edge * 46;
                for (int y = 0; y < FONT_H; y++)
                    if (g.col[x] & (1 << y))
                        badge.setPixel(screenX, top + y, bright);
            }
        }
    }

private:
    static const int COUNT = 27;
    const char* MESSAGES[COUNT] = {
        "WEOI 2026 LUXEMBOURG",
        "HELLO WORLD",
        "I LOVE C++",
        "HEY KIM",
        "LOOK NO HANDS!!",
        "TIME LIMIT EXCEEDED",
        "WRONG ANSWER",
        "SEGFAULT (CORE DUMPED)",
        "IT WORKS ON MY MACHINE",
        "// TODO: FIX LATER",
        "HAVE YOU TRIED BINARY SEARCH?",
        "GREEDY WORKS (PROBABLY)",
        "OFF BY ONE",
        "O(N!) SERIOUSLY",
        "IF IT COMPILES ALWAYS SUBMIT",
        "READ THE PROBLEM STATEMENT",
        "ALL YOUR BASE ARE BELONG TO US",
        "DO A BARREL ROLL",
        "IT'S DANGEROUS TO GO ALONE",
        "ONE DOES NOT SIMPLY DEBUG",
        "HACK THE PLANET",
        "THERE IS NO SPOON",
        "NEVER GONNA GIVE YOU UP",
        "PLEASE DO NOT FEED THE CODER",
        "240 PIXELS OF RAW POWER",
        "UP DOWN CHANGE TEXT",
        "LEFT RIGHT CHANGE SPEED",
    };
    int message = 0, pos = 0, ticks = 0, delay = 6;
};

int main() {
    Badge badge;
    Lichtkrant(badge).loop();
}
