// VU meter: a level meter whose envelope is set by 5 control points spread across
// the width. Each control point drifts smoothly with noise (pulled gently toward
// the middle so it keeps moving without sticking), and the 20 columns are linearly
// interpolated between them, so the bars rise and fall as one flowing shape rather
// than 20 independent jitters. Bright tip on each bar, dim peak-hold dot riding
// above and slowly sinking. No buttons, just watch it breathe.
#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include <cstdlib>

class VuMeter : public MainLoop {
public:
    VuMeter(Badge& badge) : MainLoop(badge) {
        badge.setFrameDelay(40);
        for (int i = 0; i < N; i++) { aLevel[i] = MAXH / 2; aTarget[i] = rand() % MAXH; }
        for (int x = 0; x < 20; x++) peak[x] = 0;
    }

    void nextFrame(Badge& badge, const FrameData&) override {
        // Drift the 5 control points smoothly (noise + mild pull to the middle).
        for (int i = 0; i < N; i++) {
            aTarget[i] += (rand() % 33) - 16 - (aTarget[i] - MAXH / 2) / 20;
            if (aTarget[i] < 0)    aTarget[i] = 0;
            if (aTarget[i] > MAXH) aTarget[i] = MAXH;
            aLevel[i] += (aTarget[i] - aLevel[i]) / 6;     // gentle easing
        }

        for (int x = 0; x < 20; x++) {
            // Map column x into control-point space and interpolate its neighbours.
            int num = x * (N - 1);                          // 0..(N-1)*19
            int seg = num / 19;                             // 0..N-1
            int rem = num - seg * 19;                       // 0..18
            int lo  = aLevel[seg];
            int hi  = aLevel[seg < N - 1 ? seg + 1 : seg];
            int h16 = lo + (hi - lo) * rem / 19;            // height in 1/16-pixel

            if (h16 > peak[x]) peak[x] = h16;
            else { peak[x] -= 3; if (peak[x] < 0) peak[x] = 0; }

            int h = h16 >> 4;                               // lit pixels, 0..12
            for (int i = 0; i < h; i++) {
                int y = 11 - i;
                badge.setPixel(x, y, i == h - 1 ? 255 : 110);   // bright tip
            }
            int py = 11 - (peak[x] >> 4);
            if (py >= 0 && py <= 11) badge.setPixel(x, py, 200); // peak-hold dot
        }
    }

private:
    static const int N    = 5;         // control points across the width
    static const int MAXH = 12 << 4;   // full-height bar, in 1/16-pixel units
    int aLevel[N], aTarget[N], peak[20];
};

int main() {
    Badge badge;
    VuMeter(badge).loop();
}
