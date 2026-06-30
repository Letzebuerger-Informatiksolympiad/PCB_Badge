// example by Eljakim Schrijvers, sorry for going crazy
// A kaleidoscope: a few bright chips tumble in one wedge and get mirrored into a
// spinning rosette, meeting their own reflections at the mirror seams. Left/Right
// change the number of mirrors, Up/Down the spin.

#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include "trig.hpp"

class Kaleidoscope : public MainLoop {
public:
    Kaleidoscope(Badge& badge) : MainLoop(badge) {
        badge.setFrameDelay(20);   // a calm ~50 fps; the chips drift gently
    }

    void nextFrame(Badge& badge, const FrameData& data) override {
        if (data.buttons.left.pressed  && segments > 4)  segments -= 2;
        if (data.buttons.right.pressed && segments < 16) segments += 2;
        if (data.buttons.up.pressed   && spin < 8)  spin++;
        if (data.buttons.down.pressed && spin > -8) spin--;

        t += 3;

        // Drift each chip: cr is its distance from the centre (1/16 px), ca its
        // position across the wedge (0..255). Both bounce off their limits, so chips
        // wander into the mirror lines, meet their reflections, and drift back.
        for (int i = 0; i < CHIPS; i++) {
            cr[i] += vr[i];
            if (cr[i] < 16 || cr[i] > 176) { vr[i] = -vr[i]; cr[i] += vr[i]; }
            ca[i] += va[i];
            if (ca[i] < 0 || ca[i] > 255) { va[i] = -va[i]; ca[i] += va[i]; }
        }

        int rot = spin * t / 16;          // how far the whole rosette has turned
        for (int y = 0; y < 12; y++) {
            for (int x = 0; x < 20; x++) {
                int dx = x - 10, dy = y - 6;
                int ang = (iatan2(dy, dx) + rot) & 255;   // 0..255 around the centre
                int r = isqrt(dx * dx + dy * dy);         // distance from the centre

                // Fold the angle into one wedge and mirror every other wedge so the
                // seams meet: that mirror symmetry is what makes it a kaleidoscope.
                int aw = ang * segments;
                int pos = aw & 255;
                if ((aw >> 8) & 1) pos = 255 - pos;

                // Light this pixel by how close it lands to each chip. The sideways
                // distance grows with r (the wedge is wider near the rim), so a chip
                // keeps the same size everywhere. Soft 1/distance glows that add up.
                int glow = 0;
                for (int i = 0; i < CHIPS; i++) {
                    int dr = r * 16 - cr[i];
                    int dt = (pos - ca[i]) * r * 3 / (segments * 8);
                    int d2 = dr * dr + dt * dt;
                    glow += 46000 / (d2 + 200);
                }

                // Faint seam lines draw the mirror facets; a dim wave fills the rest.
                int seam = pos < 255 - pos ? pos : 255 - pos;
                int edge = seam < 10 ? (10 - seam) * 3 : 0;
                int bg = 18 + (isin(r * 18 - t) >> 4);

                int b = glow + edge + bg;
                badge.setPixel(x, y, b > 255 ? 255 : b);
            }
        }
    }

private:
    static const int CHIPS = 6;
    int t = 0;
    int spin = 1;        // turn speed and direction (negative spins the other way)
    int segments = 8;    // number of mirrored wedges (kept even so they pair up)

    int cr[CHIPS] = {  48, 112, 160,  80, 176, 128 };
    int ca[CHIPS] = {  40, 200, 120,  90,  30, 220 };
    int vr[CHIPS] = {   3,  -2,   4,  -3,   2,  -4 };
    int va[CHIPS] = {   2,  -3,   5,   4,  -2,   3 };

    int isqrt(int v) {
        int r = 0;
        while ((r + 1) * (r + 1) <= v) r++;
        return r;
    }

    // Angle of (x, y) as 0..255 for a full turn (a cheap integer atan2).
    int iatan2(int y, int x) {
        if (x == 0 && y == 0) return 0;
        int ay = y < 0 ? -y : y;
        int angle = x >= 0 ? 32 - 32 * (x - ay) / (x + ay)
                           : 96 - 32 * (x + ay) / (ay - x);
        if (y < 0) angle = -angle;
        return angle & 255;
    }
};

int main() {
    Badge badge;
    Kaleidoscope(badge).loop();
}
