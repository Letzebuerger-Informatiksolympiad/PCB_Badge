// example by Eljakim Schrijvers, sorry for going crazy
// A spinning 3D wireframe shape; the far edges are drawn at half brightness.
//   Up / Down    : spin faster / slower (it never stops or reverses)
//   Left / Right : switch between a cube and a tetrahedron

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "trig.hpp"

static const int CUBE_V[8][3] = {
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
    {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1},
};
static const int CUBE_E[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    {0, 4}, {1, 5}, {2, 6}, {3, 7},
};

// A regular tetrahedron: 4 corners and an edge between every pair of them.
static const int TET_V[4][3] = { {1, 1, 1}, {1, -1, -1}, {-1, 1, -1}, {-1, -1, 1} };
static const int TET_E[6][2] = { {0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3} };

class Wireframe : public MainLoop {
public:
    Wireframe(Badge& badge) : MainLoop(badge) { badge.setFrameDelay(14); }

    void nextFrame(Badge& badge, const FrameData& data) override {
        if (data.buttons.up.pressed   && speed < 128) speed += 6;   // spin faster
        if (data.buttons.down.pressed && speed > 2) {               // slower and slower...
            speed -= 6;
            if (speed < 2) speed = 2;                               // ...but never stop or reverse
        }
        if (data.buttons.left.pressed || data.buttons.right.pressed) cube = !cube;   // swap shape

        // speed is in sixteenths of a step per frame, so it can creep very slowly.
        t += speed;
        int ax = t >> 4, ay = (ax * 3) / 2;

        const int (*V)[3] = cube ? CUBE_V : TET_V;
        const int (*E)[2] = cube ? CUBE_E : TET_E;
        int nV = cube ? 8 : 4, nE = cube ? 12 : 6;
        int px[8], py[8], pz[8];

        for (int i = 0; i < nV; i++) {
            int x = V[i][0] * 256, y = V[i][1] * 256, z = V[i][2] * 256;
            int x1 = (x * icos(ay) - z * isin(ay)) / 256;   // turn around the up axis
            int z1 = (x * isin(ay) + z * icos(ay)) / 256;
            int y1 = (y * icos(ax) - z1 * isin(ax)) / 256;  // tilt
            int z2 = (y * isin(ax) + z1 * icos(ax)) / 256;  // depth after the tilt
            px[i] = 10 + (x1 * 6) / 256;
            py[i] = 6 + (y1 * 4) / 256;
            pz[i] = z2;
        }

        for (int i = 0; i < nE; i++) {
            int a = E[i][0], b = E[i][1];
            int bright = (pz[a] + pz[b]) > 0 ? 186 : 255;   // far edges (deeper z) at ~half
            line(badge, px[a], py[a], px[b], py[b], bright);
        }
    }

private:
    int t = 0, speed = 32;   // speed 32/16 = 2 steps/frame
    bool cube = true;

    void line(Badge& badge, int x0, int y0, int x1, int y1, int bright) {
        int dx = x1 > x0 ? x1 - x0 : x0 - x1;
        int dy = -(y1 > y0 ? y1 - y0 : y0 - y1);
        int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1, err = dx + dy;
        for (;;) {
            badge.setPixel(x0, y0, bright);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }
};

int main() {
    Badge badge;
    Wireframe(badge).loop();
}
