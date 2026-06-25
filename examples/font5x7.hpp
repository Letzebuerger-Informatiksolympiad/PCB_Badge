// helper used by the examples, by Eljakim Schrijvers
// the badge SDK was developed by Luca Courte

#pragma once
#include <cstdint>

// 5x7 bitmap font. fontGlyph(c).col[x] holds a column; bit y (0..6, top=0).
struct Glyph { uint8_t col[5]; };
static const int FONT_W = 5;
static const int FONT_H = 7;

inline Glyph fontGlyph(char ch) {
    switch (ch) {
        case ' ': return {{ 0, 0, 0, 0, 0 }};
        case 'A': return {{ 126, 9, 9, 9, 126 }};
        case 'B': return {{ 127, 73, 73, 73, 54 }};
        case 'C': return {{ 62, 65, 65, 65, 34 }};
        case 'D': return {{ 127, 65, 65, 34, 28 }};
        case 'E': return {{ 127, 73, 73, 73, 65 }};
        case 'F': return {{ 127, 9, 9, 9, 1 }};
        case 'G': return {{ 62, 65, 73, 73, 122 }};
        case 'H': return {{ 127, 8, 8, 8, 127 }};
        case 'I': return {{ 65, 65, 127, 65, 65 }};
        case 'J': return {{ 48, 64, 65, 63, 1 }};
        case 'K': return {{ 127, 8, 20, 34, 65 }};
        case 'L': return {{ 127, 64, 64, 64, 64 }};
        case 'M': return {{ 127, 2, 12, 2, 127 }};
        case 'N': return {{ 127, 2, 12, 16, 127 }};
        case 'O': return {{ 62, 65, 65, 65, 62 }};
        case 'P': return {{ 127, 9, 9, 9, 6 }};
        case 'Q': return {{ 62, 65, 81, 33, 94 }};
        case 'R': return {{ 127, 9, 25, 41, 70 }};
        case 'S': return {{ 70, 73, 73, 73, 49 }};
        case 'T': return {{ 1, 1, 127, 1, 1 }};
        case 'U': return {{ 63, 64, 64, 64, 63 }};
        case 'V': return {{ 31, 32, 64, 32, 31 }};
        case 'W': return {{ 127, 32, 24, 32, 127 }};
        case 'X': return {{ 99, 20, 8, 20, 99 }};
        case 'Y': return {{ 3, 4, 120, 4, 3 }};
        case 'Z': return {{ 97, 81, 73, 69, 67 }};
        case '0': return {{ 62, 81, 77, 67, 62 }};
        case '1': return {{ 64, 66, 127, 64, 64 }};
        case '2': return {{ 66, 97, 81, 73, 70 }};
        case '3': return {{ 33, 65, 69, 75, 49 }};
        case '4': return {{ 24, 20, 18, 127, 16 }};
        case '5': return {{ 39, 69, 69, 69, 57 }};
        case '6': return {{ 62, 73, 73, 73, 48 }};
        case '7': return {{ 1, 113, 9, 5, 3 }};
        case '8': return {{ 54, 73, 73, 73, 54 }};
        case '9': return {{ 6, 73, 73, 73, 62 }};
        case '!': return {{ 0, 0, 95, 0, 0 }};
        case '.': return {{ 0, 0, 96, 0, 0 }};
        case ',': return {{ 0, 64, 48, 0, 0 }};
        case '?': return {{ 2, 1, 81, 9, 6 }};
        case ':': return {{ 0, 0, 54, 0, 0 }};
        case '-': return {{ 8, 8, 8, 8, 8 }};
        case '\'': return {{ 0, 4, 3, 0, 0 }};
        case 'a': return {{ 32, 84, 84, 84, 120 }};
        case 'b': return {{ 127, 72, 72, 72, 48 }};
        case 'c': return {{ 56, 68, 68, 68, 68 }};
        case 'd': return {{ 48, 72, 72, 72, 127 }};
        case 'e': return {{ 56, 84, 84, 84, 88 }};
        case 'f': return {{ 8, 126, 9, 1, 2 }};
        case 'g': return {{ 8, 84, 84, 84, 60 }};
        case 'h': return {{ 127, 8, 8, 8, 112 }};
        case 'i': return {{ 0, 68, 125, 64, 0 }};
        case 'j': return {{ 32, 64, 68, 61, 0 }};
        case 'k': return {{ 127, 16, 40, 68, 0 }};
        case 'l': return {{ 0, 65, 127, 64, 0 }};
        case 'm': return {{ 124, 4, 120, 4, 120 }};
        case 'n': return {{ 124, 4, 4, 4, 120 }};
        case 'o': return {{ 56, 68, 68, 68, 56 }};
        case 'p': return {{ 124, 20, 20, 20, 8 }};
        case 'q': return {{ 8, 20, 20, 20, 124 }};
        case 'r': return {{ 124, 8, 4, 4, 8 }};
        case 's': return {{ 72, 84, 84, 84, 36 }};
        case 't': return {{ 4, 63, 68, 64, 32 }};
        case 'u': return {{ 60, 64, 64, 64, 124 }};
        case 'v': return {{ 28, 32, 64, 32, 28 }};
        case 'w': return {{ 60, 64, 56, 64, 60 }};
        case 'x': return {{ 68, 40, 16, 40, 68 }};
        case 'y': return {{ 12, 80, 80, 80, 60 }};
        case 'z': return {{ 68, 100, 84, 76, 68 }};
        case '(': return {{ 0, 28, 34, 65, 0 }};
        case ')': return {{ 0, 65, 34, 28, 0 }};
        case '/': return {{ 96, 16, 8, 4, 3 }};
        case '+': return {{ 8, 8, 62, 8, 8 }};
        default: return {{ 0, 0, 0, 0, 0 }};
    }
}
