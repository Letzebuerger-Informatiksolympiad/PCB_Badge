// example by Eljakim Schrijvers, sorry for going crazy
// obviously I did not come up with the idea of tetris
// Tetris; LEFT/RIGHT move, UP rotates, hold DOWN to drop.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"
#include "digits3x5.hpp"
#include <cstdlib>
#include <cstring>

static const int PIECES[7][4][2] = {
    {{-1, 0}, {0, 0}, {1, 0}, {2, 0}},
    {{ 0, 0}, {1, 0}, {0, 1}, {1, 1}},
    {{-1, 0}, {0, 0}, {1, 0}, {0, 1}},
    {{ 0, 0}, {1, 0}, {-1, 1}, {0, 1}},
    {{-1, 0}, {0, 0}, {0, 1}, {1, 1}},
    {{-1, 0}, {0, 0}, {1, 0}, {1, 1}},
    {{-1, 0}, {0, 0}, {1, 0}, {-1, 1}},
};

class Tetris : public MainLoop {
public:
    Tetris(Badge& badge) : MainLoop(badge) { srand(2); reset(); }

    void nextFrame(Badge& badge, const FrameData& data) override {
        if (data.buttons.left.pressed)  move(-1, 0);
        if (data.buttons.right.pressed) move(1, 0);
        if (data.buttons.up.pressed)    rotate();

        int step = data.buttons.down.down ? 10 : 60;
        if (--fall <= 0) {
            fall = step;
            if (!move(0, 1)) lock();
        }

        for (int y = 0; y < 12; y++) {
            badge.setPixel(WELL - 1, y);
            badge.setPixel(WELL + 10, y);
        }
        for (int y = 0; y < 12; y++)
            for (int x = 0; x < 10; x++)
                if (board[y][x]) badge.setPixel(WELL + x, y);
        for (int i = 0; i < 4; i++)
            badge.setPixel(WELL + px + piece[i][0], py + piece[i][1]);

        drawNumber(badge, 13, 1, score);
    }

private:
    static const int WELL = 1;
    bool board[12][10];
    int piece[4][2], px, py, type, fall, score;

    void reset() { memset(board, 0, sizeof board); score = 0; spawn(); }

    void spawn() {
        type = rand() % 7;
        memcpy(piece, PIECES[type], sizeof piece);
        px = 4; py = 0; fall = 60;
        if (collides(0, 0, piece)) reset();
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
        if (type == 1) return;
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
            if (y >= 0) board[y][x] = true;
        }
        clearLines();
        spawn();
    }

    void clearLines() {
        for (int y = 11; y >= 0; y--) {
            bool full = true;
            for (int x = 0; x < 10; x++) if (!board[y][x]) full = false;
            if (full) {
                for (int row = y; row > 0; row--)
                    memcpy(board[row], board[row - 1], sizeof board[0]);
                memset(board[0], 0, sizeof board[0]);
                score++;
                y++;
            }
        }
    }
};

int main() {
    Badge badge;
    Tetris(badge).loop();
}
