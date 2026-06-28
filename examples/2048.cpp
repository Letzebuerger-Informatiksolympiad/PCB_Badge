// example by Eljakim Schrijvers, sorry for going crazy
// obviously I did not invent 2048
// 2048 where each tile's value is shown as a brightness, not a number.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include "digits3x5.hpp"
#include <cstdlib>
#include <cstring>

// Brightness for each tile value: index 1 = 2, 2 = 4, ... 11 = 2048.
static const uint8_t BR[12] = { 0, 60, 85, 110, 135, 160, 180, 200, 218, 234, 246, 255 };

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
                if (cell[r][c]) drawTile(badge, 5 + c * 3, 1 + r * 3, BR[cell[r][c]]);
    }

private:
    static const int GRID = 55;          // dim brightness of the board lines
    static const int OVER_FRAMES = 500;  // ~5 seconds at 100 fps
    int cell[4][4], score, overT;
    bool over;

    void reset() { memset(cell, 0, sizeof cell); over = false; score = 0; overT = 0; spawn(); spawn(); }

    // The board frame and the lines between the 16 cells, so the field is clear.
    void drawGrid(Badge& badge) {
        for (int x = 4; x <= 16; x++) badge.setPixel(x, 0, GRID);           // top edge
        for (int y = 0; y < 12; y++) { badge.setPixel(4, y, GRID); badge.setPixel(16, y, GRID); }
        for (int gx = 7; gx <= 13; gx += 3)
            for (int y = 1; y < 12; y++) badge.setPixel(gx, y, GRID);       // column dividers
        for (int gy = 3; gy <= 9; gy += 3)
            for (int x = 5; x <= 15; x++) badge.setPixel(x, gy, GRID);      // row dividers
    }

    void drawTile(Badge& badge, int x, int y, int bright) {
        badge.setPixel(x, y, bright);     badge.setPixel(x + 1, y, bright);
        badge.setPixel(x, y + 1, bright); badge.setPixel(x + 1, y + 1, bright);
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
