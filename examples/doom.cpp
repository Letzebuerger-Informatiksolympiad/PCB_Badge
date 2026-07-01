// doom: a Wolfenstein-style raycaster maze on a 20x12 screen.
//
// The 20 screen columns ARE 20 rays, fanned across a ~62-degree view into a
// 16x16 grid of walls. For each column we march a ray until it hits a wall, then
// draw a vertical slice whose height and brightness come from the distance: near
// walls are tall and bright, far walls short and dim, so depth reads at a glance.
// A faint floor fades in below each wall. Alternate wall cells are shaded a touch
// darker so corners and corridors stand out.
//
// Controls: Up/Down walk forward and back, Left/Right turn. You slide along walls
// instead of sticking to them. All integer math, using the badge's isin/icos
// table (angle 0..255 = one full turn), so it runs fine at 48 MHz.
#include "sdk/Badge.hpp"
#include "sdk/MainLoop.hpp"
#include "sdk/KimsPower.hpp"   // run at 48 MHz to save battery, just by including it
#include "trig.hpp"

namespace {

const int MW = 16, MH = 16;

// '#' is a wall, '.' is open. The border is solid so rays always hit something.
const char* MAP[MH] = {
    "################",
    "#..#.....#.....#",
    "#.##.###.#.###.#",
    "#....#.....#...#",
    "####.#.#####.#.#",
    "#......#...#.#.#",
    "#.####.#.#.#.#.#",
    "#.#..#...#...#.#",
    "#.#.##.#####.#.#",
    "#...#......#...#",
    "###.#.####.###.#",
    "#.....#......#.#",
    "#.#######.##.#.#",
    "#.......#..#...#",
    "#.#####.##.##..#",
    "################",
};

inline bool wall(int cx, int cy) {
    if (cx < 0 || cy < 0 || cx >= MW || cy >= MH) return true;
    return MAP[cy][cx] == '#';
}

}  // namespace

class Doom : public MainLoop {
public:
    Doom(Badge& badge) : MainLoop(badge) { badge.setFrameDelay(33); }

    void nextFrame(Badge& badge, const FrameData& data) override {
        // --- input: turn, then walk (sliding along walls per axis) ---
        if (data.buttons.left.down)  ang = (ang - 4) & 255;
        if (data.buttons.right.down) ang = (ang + 4) & 255;

        const int dx = icos(ang), dy = isin(ang);   // facing unit vector * 256
        int step = 0;
        if (data.buttons.up.down)   step =  MOVE;
        if (data.buttons.down.down) step = -MOVE;
        if (step) {
            int nx = px + dx * step / 256;
            int ny = py + dy * step / 256;
            if (!wall(nx >> 8, py >> 8)) px = nx;
            if (!wall(px >> 8, ny >> 8)) py = ny;
        }

        // --- render one vertical slice per screen column ---
        for (int sx = 0; sx < 20; sx++) {
            int ra = ang + (sx * FOV) / 19 - FOV / 2;     // this ray's angle
            int rdx = icos(ra), rdy = isin(ra);

            int t, cx = px >> 8, cy = py >> 8;
            for (t = STEP; t < MAXT; t += STEP) {
                cx = (px + rdx * t / 256) >> 8;
                cy = (py + rdy * t / 256) >> 8;
                if (wall(cx, cy)) break;
            }

            // Perpendicular distance kills the fisheye bulge. perp stays in Q8
            // cell units (256 = one cell).
            int perp = t * icos((ra - ang) & 255) / 256;
            if (perp < 64) perp = 64;                     // clamp very-close walls

            int wh = PROJ / perp;                         // wall height in pixels
            if (wh > 12) wh = 12;
            int top = (12 - wh) / 2;
            int bot = top + wh;

            int shade = 255 - (perp >> 4);
            if ((cx + cy) & 1) shade = shade * 13 / 16;    // gently darker alternate faces
            // Keep every wall above the gamma floor: brightness under ~30 maps to
            // BCM level 0 (fully off), which would punch holes in distant walls.
            if (shade < 45) shade = 45;

            for (int y = top; y < bot; y++) badge.setPixel(sx, y, shade);
            for (int y = bot; y < 12; y++) {               // floor, brighter close up
                int f = 34 + (y - bot) * 9;                // stay above the gamma floor
                if (f > 95) f = 95;
                badge.setPixel(sx, y, f);
            }
        }
    }

private:
    static const int FOV  = 44;       // view width in angle units (~62 degrees)
    static const int MOVE = 26;       // walk speed, Q8 per frame
    static const int STEP = 24;       // ray-march step, Q8 (~1/10 cell)
    static const int MAXT = 20 << 8;  // give up after 20 cells
    static const int PROJ = 10 << 8;  // wall-height projection constant

    int px = (1 << 8) | 128;          // start in the centre of cell (1,1)
    int py = (1 << 8) | 128;
    int ang = 0;                       // facing east (+x)
};

int main() {
    Badge badge;
    Doom(badge).loop();
}
