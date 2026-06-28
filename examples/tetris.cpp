// example by Eljakim Schrijvers, sorry for going crazy
// obviously I did not come up with the idea of tetris
// Tetris; LEFT/RIGHT move, UP rotates, hold DOWN to drop.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include "digits3x5.hpp"
#include <cstdlib>
#include <cstring>

// Tetris. The well is 10 columns wide in the middle of the screen.
// LEFT/RIGHT move, UP rotates, hold DOWN to drop faster.
//
// Each of the seven pieces has its own brightness (never brighter than ~75%),
// so the stack is a patchwork of grays. When a row fills up it flashes bright
// white and then fades to black before the rows above drop down.

static const int PIECES[7][4][2] = {
    {{-1, 0}, {0, 0}, {1, 0}, {2, 0}},   // I
    {{ 0, 0}, {1, 0}, {0, 1}, {1, 1}},   // O
    {{-1, 0}, {0, 0}, {1, 0}, {0, 1}},   // T
    {{ 0, 0}, {1, 0}, {-1, 1}, {0, 1}},  // S
    {{-1, 0}, {0, 0}, {0, 1}, {1, 1}},   // Z
    {{-1, 0}, {0, 0}, {1, 0}, {1, 1}},   // J
    {{-1, 0}, {0, 0}, {1, 0}, {-1, 1}},  // L
};

// One brightness per piece type, all at or below 75% (191/255).
static const uint8_t BRIGHT[7] = { 191, 100, 160, 125, 180, 110, 145 };

class Tetris : public MainLoop {
public:
    Tetris(Badge& badge) : MainLoop(badge) { srand(2); reset(); }

    void nextFrame(Badge& badge, const FrameData& data) override {
        drawWalls(badge);
        drawBoard(badge);
        drawNumber(badge, 13, 1, score);   // score on the right

        // While rows are clearing, the game freezes and the full rows flash
        // bright and fade to black; then the rows above drop in.
        if (flash > 0) {
            int level = 255 * flash / FLASH;   // fades from white to off
            for (int k = 0; k < nClear; k++)
                for (int x = 0; x < 10; x++)
                    badge.setPixel(WELL + x, clearRows[k], level);
            if (--flash == 0) collapse();
            return;
        }

        if (data.buttons.left.pressed)  move(-1, 0);
        if (data.buttons.right.pressed) move(1, 0);
        if (data.buttons.up.pressed)    rotate();

        int step = data.buttons.down.down ? 10 : 60;
        if (--fall <= 0) {
            fall = step;
            if (!move(0, 1)) lock();
        }

        for (int i = 0; i < 4; i++)
            badge.setPixel(WELL + px + piece[i][0], py + piece[i][1], BRIGHT[type]);
    }

private:
    static const int WELL = 1;
    static const int FLASH = 12;   // frames the clearing rows flash and fade
    uint8_t board[12][10];         // 0 = empty, otherwise the locked brightness
    int piece[4][2], px, py, type, fall, score;
    int clearRows[12], nClear, flash;

    void reset() { memset(board, 0, sizeof board); score = 0; nClear = 0; flash = 0; spawn(); }

    void drawWalls(Badge& badge) {
        for (int y = 0; y < 12; y++) {
            badge.setPixel(WELL - 1, y, 90);     // dim frame around the well
            badge.setPixel(WELL + 10, y, 90);
        }
    }

    void drawBoard(Badge& badge) {
        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 10; x++)
                if (board[y][x]) badge.setPixel(WELL + x, y, board[y][x]);
    }

    void spawn() {
        type = rand() % 7;
        memcpy(piece, PIECES[type], sizeof piece);
        px = 4; py = 0; fall = 60;
        if (collides(0, 0, piece)) reset();   // stack reached the top
    }

    bool collides(int dx, int dy, int cells[4][2]) {
        for (int i = 0; i < 4; i++) {
            int x = px + cells[i][0] + dx, y = py + cells[i][1] + dy;
            if (x < 0 || x >= 10 || y >= 12) return true;
            if (y >= 0 && board[y][x]) return true;
        }
        return false;
    }

    bool move(int dx, int dy) {
        if (collides(dx, dy, piece)) return false;
        px += dx; py += dy;
        return true;
    }

    void rotate() {
        if (type == 1) return;   // the square does not turn
        int turned[4][2];
        for (int i = 0; i < 4; i++) {
            turned[i][0] = -piece[i][1];
            turned[i][1] = piece[i][0];
        }
        if (!collides(0, 0, turned)) memcpy(piece, turned, sizeof piece);
    }

    void lock() {
        for (int i = 0; i < 4; i++) {
            int x = px + piece[i][0], y = py + piece[i][1];
            if (y >= 0) board[y][x] = BRIGHT[type];   // keep the piece's gray
        }
        nClear = 0;
        for (int y = 0; y < 12; y++) {
            bool full = true;
            for (int x = 0; x < 10; x++) if (!board[y][x]) full = false;
            if (full) clearRows[nClear++] = y;
        }
        if (nClear) flash = FLASH;   // start the flash-and-fade, spawn after
        else        spawn();
    }

    void collapse() {
        for (int k = 0; k < nClear; k++) {
            int y = clearRows[k];     // clearRows is in top-to-bottom order
            for (int row = y; row > 0; row--)
                memcpy(board[row], board[row - 1], sizeof board[0]);
            memset(board[0], 0, sizeof board[0]);
            score++;
        }
        nClear = 0;
        spawn();
    }
};

int main() {
    Badge badge;
    Tetris(badge).loop();
}
