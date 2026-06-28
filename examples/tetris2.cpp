// example by Eljakim Schrijvers, sorry for going crazy
// obviously I did not come up with the idea of tetris
// Sideways Tetris that falls to the left; shows your score when you lose.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include "digits3x5.hpp"
#include <cstdlib>
#include <cstring>

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

// The word SCORE, drawn by hand. The digits come from digits3x5.hpp.
static const char* SCORE_WORD[5] = {
    "### ### ### ##  ###",
    "#   #   # # # # #  ",
    "### #   # # ##  ###",
    "  # #   # # # # #  ",
    "### ### ### # # ###",
};

class Tetris2 : public MainLoop {
public:
    Tetris2(Badge& badge) : MainLoop(badge) { srand(4); reset(); }

    void nextFrame(Badge& badge, const FrameData& data) override {
        bool anyButton = data.buttons.up.pressed || data.buttons.down.pressed ||
                         data.buttons.left.pressed || data.buttons.right.pressed;
        if (over) {
            if (anyButton) reset();
            drawScore(badge);
            return;
        }

        if (data.buttons.up.pressed)    move(0, -1);
        if (data.buttons.down.pressed)  move(0, 1);
        if (data.buttons.right.pressed) rotate(1);

        int step = data.buttons.left.down ? 6 : FALL;   // hold LEFT to drop faster
        if (--fall <= 0) {
            fall = step;
            if (!move(-1, 0)) lock();
        }

        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 20; x++)
                if (board[y][x]) badge.setPixel(x, y, board[y][x]);
        for (int i = 0; i < 4; i++)
            badge.setPixel(px + piece[i][0], py + piece[i][1], BRIGHT[type]);
    }

private:
    static const int FALL = 22;
    uint8_t board[12][20];   // 0 = empty, otherwise the locked brightness
    bool over;
    int piece[4][2], px, py, type, fall, score;

    void reset() { memset(board, 0, sizeof board); score = 0; over = false; spawn(); }

    void spawn() {
        type = rand() % 7;
        memcpy(piece, PIECES[type], sizeof piece);
        px = 17; py = 5; fall = FALL;
        if (collides(0, 0, piece)) over = true;
    }

    bool collides(int dx, int dy, int cells[4][2]) {
        for (int i = 0; i < 4; i++) {
            int x = px + cells[i][0] + dx, y = py + cells[i][1] + dy;
            if (x < 0 || x >= 20 || y < 0 || y >= 12) return true;
            if (board[y][x]) return true;
        }
        return false;
    }

    bool move(int dx, int dy) {
        if (collides(dx, dy, piece)) return false;
        px += dx; py += dy;
        return true;
    }

    void rotate(int dir) {
        if (type == 1) return;
        int turned[4][2];
        for (int i = 0; i < 4; i++) {
            turned[i][0] = dir > 0 ? -piece[i][1] : piece[i][1];
            turned[i][1] = dir > 0 ? piece[i][0] : -piece[i][0];
        }
        if (!collides(0, 0, turned)) memcpy(piece, turned, sizeof piece);
    }

    void lock() {
        for (int i = 0; i < 4; i++) board[py + piece[i][1]][px + piece[i][0]] = BRIGHT[type];
        clearColumns();
        spawn();
    }

    void clearColumns() {
        for (int x = 0; x < 20; x++) {
            bool full = true;
            for (int y = 0; y < 12; y++) if (!board[y][x]) full = false;
            if (full) {
                for (int k = x; k < 19; k++)
                    for (int y = 0; y < 12; y++) board[y][k] = board[y][k + 1];
                for (int y = 0; y < 12; y++) board[y][19] = false;
                score++;
                x--;
            }
        }
    }

    void drawScore(Badge& badge) {
        for (int r = 0; r < 5; r++)
            for (int c = 0; c < 19; c++)
                if (SCORE_WORD[r][c] == '#') badge.setPixel(c, 1 + r);

        drawNumber(badge, (20 - numberWidth(score)) / 2, 7, score);
    }
};

int main() {
    Badge badge;
    Tetris2(badge).loop();
}
