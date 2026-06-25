// example by Eljakim Schrijvers, sorry for going crazy
// the SDK that this was built on was developed by Luca Courte
// Scrolling text; LEFT and RIGHT change the speed.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "font5x7.hpp"
#include <cstring>

class Hello5 : public MainLoop {
public:
    Hello5(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData& data) override {
        if (data.buttons.right.pressed && delay > 1)  delay--;
        if (data.buttons.left.pressed  && delay < 24) delay++;

        const char* text = "HELLO WORLD";
        int textWidth = (int)strlen(text) * (FONT_W + 1);

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
                for (int y = 0; y < FONT_H; y++)
                    if (g.col[x] & (1 << y))
                        badge.setPixel(screenX, top + y);
            }
        }
    }

private:
    int pos = 0, ticks = 0, delay = 6;
};

int main() {
    Badge badge;
    Hello5(badge).loop();
}
