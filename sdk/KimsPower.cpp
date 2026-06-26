#define KIMSPOWER_NO_AUTO_POWER_SAVING   // don't power-save just by linking this file
#include "KimsPower.hpp"

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <hardware/vreg.h>
#include <pico/stdlib.h>

namespace {
const uint OE = 10;            // '595 output-enable, active-low; Badge holds it at 0
const uint16_t PWM_WRAP = 255;
bool _pwmReady = false;
}

void KimsPower::enablePowerSaving(uint32_t systemClockKhz) {
    vreg_set_voltage(VREG_VOLTAGE_0_90);
    sleep_ms(2);
    set_sys_clock_khz(systemClockKhz, true);
}

void KimsPower::setBrightness(uint8_t level) {
    uint slice = pwm_gpio_to_slice_num(OE);
    uint channel = pwm_gpio_to_channel(OE);
    if (!_pwmReady) {
        gpio_set_function(OE, GPIO_FUNC_PWM);
        pwm_config cfg = pwm_get_default_config();
        pwm_config_set_clkdiv(&cfg, 4.0f);   // ~46 kHz, above the column-scan rate
        pwm_config_set_wrap(&cfg, PWM_WRAP);
        pwm_init(slice, &cfg, true);
        _pwmReady = true;
    }
    pwm_set_chan_level(slice, channel, PWM_WRAP - level);   // OE active-low, so invert
}
