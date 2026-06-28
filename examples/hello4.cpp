// example by Eljakim Schrijvers, sorry for going crazy
// Write text using the built-in font.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"
#include "font5x7.hpp"

class Hello4 : public MainLoop {
public:
    Hello4(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData& data) override {
        const char* text = "HELLO WORLD";
        int top = (12 - FONT_H) / 2;

        for (int i = 0; text[i]; i++) {
            Glyph g = fontGlyph(text[i]);
            int letterX = i * (FONT_W + 1);
            for (int x = 0; x < FONT_W; x++)
                for (int y = 0; y < FONT_H; y++)
                    if (g.col[x] & (1 << y))
                        badge.setPixel(letterX + x, top + y);
        }
    }
};

int main() {
    Badge badge;
    Hello4(badge).loop();
}
