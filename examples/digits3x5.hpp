// helper used by the examples, by Eljakim Schrijvers
// the badge SDK was developed by Luca Courte

#pragma once
#include "sdk/Badge.hpp"

// Small 3-wide, 5-tall digits for showing numbers like a score.
static const char* DIGIT3X5[10][5] = {
    {"###", "# #", "# #", "# #", "###"}, {" # ", "## ", " # ", " # ", "###"},
    {"###", "  #", "###", "#  ", "###"}, {"###", "  #", "###", "  #", "###"},
    {"# #", "# #", "###", "  #", "  #"}, {"###", "#  ", "###", "  #", "###"},
    {"###", "#  ", "###", "# #", "###"}, {"###", "  #", "  #", "  #", "  #"},
    {"###", "# #", "###", "# #", "###"}, {"###", "# #", "###", "  #", "###"},
};

inline void drawDigit(Badge& badge, int x, int y, int d) {
    for (int r = 0; r < 5; r++)
        for (int c = 0; c < 3; c++)
            if (DIGIT3X5[d][r][c] == '#') badge.setPixel(x + c, y + r);
}

// Width in pixels that `value` will take when drawn.
inline int numberWidth(int value) {
    int n = 1;
    while (value >= 10) { value /= 10; n++; }
    return n * 4 - 1;
}

// Draw `value` with its left edge at x, y.
inline void drawNumber(Badge& badge, int x, int y, int value) {
    int stack[6], n = 0;
    do { stack[n++] = value % 10; value /= 10; } while (value > 0 && n < 6);
    for (int i = 0; i < n; i++) drawDigit(badge, x + i * 4, y, stack[n - 1 - i]);
}
