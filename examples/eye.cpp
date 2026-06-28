// example by Eljakim Schrijvers, sorry for going crazy
#include "sdk/Badge.hpp"
#include "eye_anim.hpp"

// A blinking, glancing eye, played back in full grayscale. The animation is 72
// frames (EYE_ANIM[frame][row][col], 0..255). The badge refreshes the display in
// the background, so we just draw a frame, publish it, and hold it for FRAME_MS.
const int FRAME_MS = 55;          // ~18 fps

// The animation bakes a top-to-bottom gradient into the grey surround (~122 at
// the top, ~168 at the bottom), so scaling it keeps the gradient and the bottom
// looks brighter. Instead we flatten the whole grey mid-band to ONE level, so
// the surround is a uniform brightness. The bright eye (>= EYE_THRESHOLD, the
// sclera) and the dark pupil (< PUPIL_MAX) are left untouched so the eye and its
// glance still read. Tune SURROUND_LEVEL down (toward 0) for a darker surround.
const int EYE_THRESHOLD  = 190;
const int PUPIL_MAX      = 80;
const int SURROUND_LEVEL = 40;    // a dim lit level (0 = off). The scale is gamma-corrected, so small values stay genuinely dim.

int main() {
    Badge badge;
    badge.setFrameDelay(FRAME_MS);
    int frame = 0;
    while (true) {
        for (int y = 0; y < EYE_LINES; y++)
            for (int x = 0; x < EYE_PIXELS; x++) {
                int v = EYE_ANIM[frame][y][x];
                if (v >= PUPIL_MAX && v < EYE_THRESHOLD) v = SURROUND_LEVEL;
                badge.setPixel(x, y, v);
            }
        badge.updatePixels();        // publish; the background refresh keeps it lit
        frame = (frame + 1) % EYE_FRAMES;
    }
}
