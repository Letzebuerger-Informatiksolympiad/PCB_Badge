// example by Eljakim Schrijvers, sorry for going crazy
// obviously I did not invent 2048
// 2048 where each tile's value is shown as a brightness, not a number.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include "digits3x5.hpp"
#include <cstdlib>
#include <cstring>

// The three brightness bands, well separated so they are easy to tell apart.
static const uint8_t BAND[3] = { 100, 175, 255 };
// How the 2x2 tile fills up (bits: 1=top-left, 2=top-right, 4=bottom-left,
// 8=bottom-right): one pixel, two, three, then all four.
static const uint8_t PAT[4] = { 0b0001, 0b0101, 0b0111, 0b1111 };

class Game2048 : public MainLoop {
public:
    Game2048(Badge& badge) : MainLoop(badge) { srand(8); reset(); }

    void nextFrame(Badge& badge, const FrameData& data) override {
        bool any = data.buttons.up.pressed || data.buttons.down.pressed ||
                   data.buttons.left.pressed || data.buttons.right.pressed;

        if (over) {
            drawNumber(badge, (20 - numberWidth(score)) / 2, 3, score);
            int remain = OVER_FRAMES - overT;                 // 500..0 over five seconds
            int barLen = 20 * remain / OVER_FRAMES;
            for (int x = 0; x < barLen; x++) badge.setPixel(x, 11, 120);
            if (overT < OVER_FRAMES) overT++;                 // drain the bar first
            else if (any) reset();                            // then wait for a button
            return;
        }

        bool moved = false;
        if (data.buttons.left.pressed)  moved = move(0);
        if (data.buttons.right.pressed) moved = move(1);
        if (data.buttons.up.pressed)    moved = move(2);
        if (data.buttons.down.pressed)  moved = move(3);
        if (moved) { spawn(); if (!canMove()) over = true; }

        drawGrid(badge);
        for (int r = 0; r < 4; r++)
            for (int c = 0; c < 4; c++)
                if (cell[r][c]) drawTile(badge, 5 + c * 3, r * 3, cell[r][c]);
    }

private:
    static const int GRID = 55;          // dim brightness of the board lines
    static const int OVER_FRAMES = 500;  // ~5 seconds at 100 fps
    int cell[4][4], score, overT;
    bool over;

    void reset() { memset(cell, 0, sizeof cell); over = false; score = 0; overT = 0; spawn(); spawn(); }

    // Only the lines between the 16 cells, no outer border.
    void drawGrid(Badge& badge) {
        for (int gx = 7; gx <= 13; gx += 3)
            for (int y = 0; y <= 10; y++) badge.setPixel(gx, y, GRID);      // column dividers
        for (int gy = 2; gy <= 8; gy += 3)
            for (int x = 5; x <= 15; x++) badge.setPixel(x, gy, GRID);      // row dividers
    }

    // Draw one tile: pick the brightness band and how many pixels from its value.
    void drawTile(Badge& badge, int x, int y, int exp) {
        if (exp >= 12) {                        // 4096 and beyond: a bright slash
            badge.setPixel(x + 1, y, 255);      // top-right down...
            badge.setPixel(x, y + 1, 255);      // ...to bottom-left
            return;
        }
        int mask = PAT[(exp - 1) % 4];
        int bright = BAND[(exp - 1) / 4];
        if (mask & 1) badge.setPixel(x, y, bright);
        if (mask & 2) badge.setPixel(x + 1, y, bright);
        if (mask & 4) badge.setPixel(x, y + 1, bright);
        if (mask & 8) badge.setPixel(x + 1, y + 1, bright);
    }

    void spawn() {
        int empty[16], n = 0;
        for (int r = 0; r < 4; r++)
            for (int c = 0; c < 4; c++) if (!cell[r][c]) empty[n++] = r * 4 + c;
        if (!n) return;
        int p = empty[rand() % n];
        cell[p / 4][p % 4] = (rand() % 10 == 0) ? 2 : 1;   // mostly 2, sometimes 4
    }

    // Read/write one line in the chosen direction so one slide routine serves all.
    int& at(int dir, int k, int i) {
        if (dir == 0) return cell[k][i];        // left:  toward column 0
        if (dir == 1) return cell[k][3 - i];    // right
        if (dir == 2) return cell[i][k];        // up:    toward row 0
        return cell[3 - i][k];                  // down
    }

    bool move(int dir) {
        bool moved = false;
        for (int k = 0; k < 4; k++) {
            int line[4];
            for (int i = 0; i < 4; i++) line[i] = at(dir, k, i);
            if (slide(line)) moved = true;
            for (int i = 0; i < 4; i++) at(dir, k, i) = line[i];
        }
        return moved;
    }

    bool slide(int* a) {
        int packed[4] = {0, 0, 0, 0}, n = 0;
        for (int i = 0; i < 4; i++) if (a[i]) packed[n++] = a[i];
        for (int i = 0; i + 1 < n; i++)
            if (packed[i] == packed[i + 1]) { packed[i]++; packed[i + 1] = 0; score += 1 << packed[i]; }
        int out[4] = {0, 0, 0, 0}, m = 0;
        for (int i = 0; i < 4; i++) if (packed[i]) out[m++] = packed[i];
        bool moved = false;
        for (int i = 0; i < 4; i++) { if (a[i] != out[i]) moved = true; a[i] = out[i]; }
        return moved;
    }

    bool canMove() {
        for (int r = 0; r < 4; r++)
            for (int c = 0; c < 4; c++) {
                if (!cell[r][c]) return true;
                if (c < 3 && cell[r][c] == cell[r][c + 1]) return true;
                if (r < 3 && cell[r][c] == cell[r + 1][c]) return true;
            }
        return false;
    }
};

int main() {
    Badge badge;
    Game2048(badge).loop();
}
