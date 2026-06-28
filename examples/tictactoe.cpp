// example by Luca Courte
// Two-player tic-tac-toe.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/Ticker.hpp"
#include "sdk/AnimationWEOI.hpp"

void drawHline(Badge& badge, int y) {
    for(int i = 0; i < 11; ++i) {
        badge.setPixel(i, y);
    }
}
void drawVline(Badge& badge,int x) {
    for(int i = 0; i < 11; ++i) {
        badge.setPixel(x, i);
    }
}

void draw_N(int x, int y, Badge& badge) {
    for(int i = 0; i < 5; ++i) {
        badge.setPixel(x-2, y-2+i);
        badge.setPixel(x+2, y-2+i);
    }
    badge.setPixel(x-1, y-1);
    badge.setPixel(x, y);
    badge.setPixel(x+1, y+1);
}

void drawX(Badge& badge,int middle_x, int middle_y) {
    badge.setPixel(middle_x - 1, middle_y - 1, 255);
    badge.setPixel(middle_x - 1, middle_y + 1, 255);
    badge.setPixel(middle_x, middle_y, 255);
    badge.setPixel(middle_x + 1, middle_y - 1, 255);
    badge.setPixel(middle_x + 1, middle_y + 1, 255);
}

void drawO(Badge& badge,int middle_x, int middle_y) {
    badge.setPixel(middle_x - 1, middle_y, 255);
    badge.setPixel(middle_x + 1, middle_y, 255);
    badge.setPixel(middle_x, middle_y - 1, 255);
    badge.setPixel(middle_x, middle_y + 1, 255);
}

class Tictactoe : public MainLoop {
public:
    Tictactoe(Badge& badge) : MainLoop(badge), cursor_ticker(20) {}

    void nextFrame(Badge& badge, const FrameData& data) override
    {
        if(check_winner() == 1) {
            drawX(badge, 9, 2);

            draw_W(4, 8, badge);
            draw_O(10, 8, badge);
            draw_N(15, 8, badge);
            return;
        }
        if(check_winner() == -1) {
            drawO(badge, 9, 2);

            draw_W(4, 8, badge);
            draw_O(10, 8, badge);
            draw_N(15, 8, badge);
            return;
        }

        if(data.buttons.up.pressed) {
            cursor = (cursor + 1) % 9;
        }
        if (data.buttons.down.pressed) {
            cursor = (9 + cursor - 1) % 9;
        }
        if (data.buttons.left.pressed) {
            if (slots[cursor] == 0) {
                slots[cursor] = player;
                cursor = 0;
                player = -player;
            }
        }
        cursor_ticker.tick();
        if(cursor_ticker.risen()) {
            cursor_on = !cursor_on;
        }

        if(!data.buttons.right.down) {
            drawHline(badge, 3);
            drawHline(badge, 7);
            drawVline(badge, 3);
            drawVline(badge, 7);
        }
        for(int i = 0; i < 9; ++i) {
            int middle_x = 1 + (i % 3) * 4;
            int middle_y = 1 + (i / 3) * 4;
            if(slots[i] == 1) {
                drawX(badge, middle_x, middle_y);
            }
            else if(slots[i] == -1) {
                drawO(badge, middle_x, middle_y);
            }
        }
        if (cursor_on) {
            badge.setPixel(1 + (cursor % 3)*4, 1 + (cursor / 3)*4, 255);
        } else {
            badge.setPixel(1 + (cursor % 3)*4, 1 + (cursor / 3)*4, 0);
        }
        if (player == 1) {
            drawX(badge, 16, 5);
        } else {
            drawO(badge, 16, 5);
        }
    }

    int check_winner() {
        if (slots[0] != 0 && slots[0] == slots[1] && slots[1] == slots[2]) return slots[0];
        if (slots[3] != 0 && slots[3] == slots[4] && slots[4] == slots[5]) return slots[3];
        if (slots[6] != 0 && slots[6] == slots[7] && slots[7] == slots[8]) return slots[6];

        if (slots[0] != 0 && slots[0] == slots[3] && slots[3] == slots[6]) return slots[0];
        if (slots[1] != 0 && slots[1] == slots[4] && slots[4] == slots[7]) return slots[1];
        if (slots[2] != 0 && slots[2] == slots[5] && slots[5] == slots[8]) return slots[2];

        if (slots[0] != 0 && slots[0] == slots[4] && slots[4] == slots[8]) return slots[0];
        if (slots[2] != 0 && slots[2] == slots[4] && slots[4] == slots[6]) return slots[2];

        return 0;
    }
private:
    int cursor = 0;
    bool cursor_on = false;
    Ticker cursor_ticker;
    int slots[9] = { 0 };
    int player = 1;
};

int main() {
    Badge badge;

    Tictactoe(badge).loop();
}
