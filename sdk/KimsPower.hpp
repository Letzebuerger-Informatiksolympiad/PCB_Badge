#pragma once

#include <cstdint>

// Battery tuning. The main loop busy-waits, so the CPU runs flat out and clock
// speed dominates current draw. #include this header to drop the clock
// automatically (the static init below); leave it out to keep full speed.
// Define KIMSPOWER_NO_AUTO_POWER_SAVING to opt a translation unit out.
class KimsPower {
public:
    // Lower core voltage + system clock. 48 MHz stays responsive; safe to repeat.
    static void enablePowerSaving(uint32_t systemClockKhz);

    // Global LED brightness via PWM on the '595 OE pin. 0 = off, 255 = full.
    // Call after constructing Badge (its ctor drives OE directly).
    static void setBrightness(uint8_t level);
};

#ifndef KIMSPOWER_NO_AUTO_POWER_SAVING
#ifndef KIMSPOWER_SYSTEM_CLOCK_KHZ
#define KIMSPOWER_SYSTEM_CLOCK_KHZ 48000
#endif
namespace {
struct KimsPowerAutoInit {
    KimsPowerAutoInit() { KimsPower::enablePowerSaving(KIMSPOWER_SYSTEM_CLOCK_KHZ); }
};
KimsPowerAutoInit _kimsPowerAutoInit;
}
#endif
