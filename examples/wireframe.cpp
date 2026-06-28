// example by Eljakim Schrijvers, sorry for going crazy
// A spinning 3D wireframe cube; the far edges are drawn at half brightness.
//   Up / Down    : spin faster / slower (and past zero, spin the other way)
//   Left / Right : turn the cube by hand

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "trig.hpp"

static const int CORNER[8][3] = {
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
    {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1},
};
static const int EDGE[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    {0, 4}, {1, 5}, {2, 6}, {3, 7},
};

class Wireframe : public MainLoop {
public:
    Wireframe(Badge& badge) : MainLoop(badge) { badge.setFrameDelay(14); }

    void nextFrame(Badge& badge, const FrameData& data) override {
        if (data.buttons.up.pressed   && speed < 8)  speed++;   // spin faster
        if (data.buttons.down.pressed && speed > -8) speed--;   // slower, then reverse
        if (data.buttons.left.down)  yaw -= 4;                  // turn it by hand
        if (data.buttons.right.down) yaw += 4;

        t += speed;
        int ax = t, ay = (t * 3) / 2 + yaw;
        int px[8], py[8], pz[8];

        for (int i = 0; i < 8; i++) {
            int x = CORNER[i][0] * 256, y = CORNER[i][1] * 256, z = CORNER[i][2] * 256;
            int x1 = (x * icos(ay) - z * isin(ay)) / 256;   // turn around the up axis
            int z1 = (x * isin(ay) + z * icos(ay)) / 256;
            int y1 = (y * icos(ax) - z1 * isin(ax)) / 256;  // tilt
            int z2 = (y * isin(ax) + z1 * icos(ax)) / 256;  // depth after the tilt
            px[i] = 10 + (x1 * 6) / 256;
            py[i] = 6 + (y1 * 4) / 256;
            pz[i] = z2;
        }

        for (int i = 0; i < 12; i++) {
            int a = EDGE[i][0], b = EDGE[i][1];
            int bright = (pz[a] + pz[b]) > 0 ? 186 : 255;   // far edges (deeper z) at ~half
            line(badge, px[a], py[a], px[b], py[b], bright);
        }
    }

private:
    int t = 0, speed = 2, yaw = 0;

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
