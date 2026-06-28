// example by Eljakim Schrijvers, sorry for going crazy
// A soft square that glides around with antialiased (area-coverage) edges.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it

class Glide : public MainLoop {
public:
    Glide(Badge& badge) : MainLoop(badge) {}

    void nextFrame(Badge& badge, const FrameData&) override {
        fx += vx; fy += vy;
        if (fx < 0)    { fx = 0;    vx = -vx; }   // bounce off the walls
        if (fx > maxX) { fx = maxX; vx = -vx; }
        if (fy < 0)    { fy = 0;    vy = -vy; }
        if (fy > maxY) { fy = maxY; vy = -vy; }

        for (int py = 0; py < 12; py++) {
            int oy = overlap(fy, py);
            if (!oy) continue;
            for (int px = 0; px < 20; px++) {
                int ox = overlap(fx, px);
                if (!ox) continue;
                int cover = ox * oy / 256;        // covered area, 0..256
                badge.setPixel(px, py, cover * 255 / 256);
            }
        }
    }

private:
    static const int S = 4 * 256 + 128;           // a 4.5 pixel square (units: 1/256 px)
    static const int maxX = 20 * 256 - S;
    static const int maxY = 12 * 256 - S;
    int fx = 3 * 256, fy = 2 * 256, vx = 47, vy = 31;

    // How much of pixel cell `p` the square [f, f+S] covers, as 0..256.
    int overlap(int f, int p) {
        int lo = p * 256, hi = lo + 256;
        int a = f > lo ? f : lo;
        int b = (f + S) < hi ? (f + S) : hi;
        int o = b - a;
        return o > 0 ? o : 0;
    }
};

int main() {
    Badge badge;
    Glide(badge).loop();
}
