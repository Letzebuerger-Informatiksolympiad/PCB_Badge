// helper used by the examples, by Eljakim Schrijvers
// the badge SDK was developed by Luca Courte

#pragma once

// Integer sine/cosine. Angle is 0..255 = one full turn; result in [-256,256].
inline int isin(int a) {
    static const int T[65] = { 0, 6, 13, 19, 25, 31, 38, 44, 50, 56, 62, 68, 74, 80, 86, 92, 98, 104, 109, 115, 121, 126, 132, 137, 142, 147, 152, 157, 162, 167, 172, 177, 181, 185, 190, 194, 198, 202, 206, 209, 213, 216, 220, 223, 226, 229, 231, 234, 237, 239, 241, 243, 245, 247, 248, 250, 251, 252, 253, 254, 255, 255, 256, 256, 256 };
    a &= 255;
    if (a < 64) return T[a];
    if (a < 128) return T[128 - a];
    if (a < 192) return -T[a - 128];
    return -T[256 - a];
}
inline int icos(int a) { return isin(a + 64); }
