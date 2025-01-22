#include "io.h"
#include "board_def.h"
#include "hardware/pwm.h"
#include "fan.h"

static constexpr int fan_ref_frequency = 10000; 
static constexpr int fan_pwm_wrap = 99;
static int current_dc = 0;

void init_fan() {
    // Set GPIOs to the PWM function
    gpio_set_function(IO_FAN_PWM, GPIO_FUNC_PWM);

    auto slice = pwm_gpio_to_slice_num(IO_FAN_PWM);
    // Get the PWM configuration 
    pwm_config config = pwm_get_default_config(); 
    // Set the divisor to 1 (no division, full speed) 
    pwm_config_set_clkdiv_int(&config, 125000000/(fan_ref_frequency*(fan_pwm_wrap+1)));
    // Set the PWM wrap value
    pwm_config_set_wrap(&config, fan_pwm_wrap);
    // Init channels 
    pwm_init(slice, &config, true);

    set_fan_speed(50);

    // Enable the PWM slice
    pwm_set_enabled(slice, true);
}

void set_fan_speed(int duty_cycle) {
    if (duty_cycle < 0)
        duty_cycle = 0;
    if (duty_cycle > 100)
        duty_cycle = 100;
    pwm_set_gpio_level(IO_FAN_PWM, duty_cycle);
    current_dc = duty_cycle;
}

int get_fan_speed() {
    return current_dc;
}

void set_fan(bool onoff) {
    gpio_set(IO_FAN_PWR_C, onoff);  
}
