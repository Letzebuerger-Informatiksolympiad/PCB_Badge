#include "Badge.hpp"

#include <hardware/gpio.h>
#include <pico/stdlib.h>

namespace {

// ── Pins ─────────────────────────────────────────
const uint BTN_UP    = 0;
const uint BTN_RIGHT = 1;
const uint BTN_LEFT  = 2;
const uint BTN_DOWN  = 3;

const uint DIN  = 9;
const uint OE   = 10;
const uint CLK  = 11;
const uint RCLK = 12;

// ── Bit maps ─────────────────────────────────────
const uint8_t COL_BIT[20] = {
  11,10,9,23,22,21,20,19,18,17,
  31,30,29,28,27,26,25,24,8,16
};

const uint8_t ROW_BIT[12] = {
  12,13,14,15,0,4,3,2,1,5,6,7
};

// ── Low-level write ─────────────────────────────
void write32(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        gpio_put(DIN, (value >> i) & 1);

        gpio_put(CLK, 1);
        sleep_us(1);
        gpio_put(CLK, 0);
    }

    gpio_put(RCLK, 1);
    sleep_us(1);
    gpio_put(RCLK, 0);
}

// ── Build shift word ────────────────────────────
uint32_t buildWord(uint16_t rowMask, int col) {
    uint32_t word = 0UL;

    // All columns OFF (HIGH)
    for (int c = 0; c < 20; c++) {
        word |= (1UL << COL_BIT[c]);
    }

    // One column ON (LOW)
    if (col >= 0) {
        word &= ~(1UL << COL_BIT[col]);
    }

    // Rows ON (HIGH)
    for (int r = 0; r < 12; r++) {
        if (rowMask & (1 << r)) {
        word |= (1UL << ROW_BIT[r]);
        }
    }

    return word;
}

}

Badge::Badge() {
    stdio_init_all();
    // Matrix
    gpio_init(DIN);
    gpio_set_dir(DIN, GPIO_OUT);

    gpio_init(CLK);
    gpio_set_dir(CLK, GPIO_OUT);

    gpio_init(RCLK);
    gpio_set_dir(RCLK, GPIO_OUT);

    gpio_init(OE);
    gpio_set_dir(OE, GPIO_OUT);

    gpio_put(OE, 0);

    write32(buildWord(0, -1));

    // Buttons
    gpio_init(BTN_UP);
    gpio_set_dir(BTN_UP, GPIO_IN);
    gpio_pull_up(BTN_UP);

    gpio_init(BTN_DOWN);
    gpio_set_dir(BTN_DOWN, GPIO_IN);
    gpio_pull_up(BTN_DOWN);

    gpio_init(BTN_LEFT);
    gpio_set_dir(BTN_LEFT, GPIO_IN);
    gpio_pull_up(BTN_LEFT);

    gpio_init(BTN_RIGHT);
    gpio_set_dir(BTN_RIGHT, GPIO_IN);
    gpio_pull_up(BTN_RIGHT);
}


void Badge::clearPixels() {
  for (int y = 0; y < 12; y++) {
    for (int x = 0; x < 20; x++) {
      _framebuffer[y][x] = false;
    }
  }
}
void Badge::setPixel(unsigned x, unsigned y, bool on) {
  if (x < 0 || x >= 20 || y < 0 || y >= 12) return;
  _framebuffer[y][x] = on;
}
void Badge::updatePixels() const {
  for (int col = 0; col < 20; col++) {
    uint16_t rowMask = 0;

    for (int row = 0; row < 12; row++) {
      if (_framebuffer[row][col]) {
        rowMask |= (1 << row);
      }
    }

    write32(buildWord(rowMask, col));
    sleep_us(500);
  }
}

bool Badge::isPressed(Key key) const {
    switch(key) {
        case Key::Up:
            return !gpio_get(BTN_UP);
        case Key::Down:
            return !gpio_get(BTN_DOWN);
        case Key::Left:
            return !gpio_get(BTN_LEFT);
        case Key::Right:
            return !gpio_get(BTN_RIGHT);
        default:
            return false;
    }
}
