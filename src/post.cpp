#include "board_def.h"
#include "io.h"
#include "adc.h"
#include "fan.h"
#include "flash.h"
#include "ref_pwm.h"
#include "cli_out.h"
#include "analog.h"
#include "post.h"

constexpr float trigger_tolerance{0.035};

static post_result_t post_result;

bool post() {
    // 1. Turn the fan off, laser off and measure dark voltage on sensors
    set_fan(false);
    task_sleep_ms(500);
    set_laser(0, false);
    set_laser(1, false);
    task_sleep_ms(20);
    post_result.dark_v[0] = measure_avg_voltage(0, 10).first;
    post_result.dark_v[1] = measure_avg_voltage(1, 10).first;

    // 2.1. Turn the laser1 on and measure ambient light through sensor1
    set_laser(0, true);
    set_laser(1, false);
    task_sleep_ms(20);
    post_result.ambient_single[0] = measure_avg_voltage(0, 10).first;

    // 2.1. Turn the laser2 on and measure ambient light through sensor2
    set_laser(0, false);
    set_laser(1, true);
    task_sleep_ms(20);
    post_result.ambient_single[1] = measure_avg_voltage(1, 10).first;

    // 3.1. Turn both lasers on and measure ambient light through both sensors
    set_laser(0, true);
    set_laser(1, true);
    task_sleep_ms(20);
    post_result.ambient_both[0] = measure_avg_voltage(0, 10).first;
    post_result.ambient_both[1] = measure_avg_voltage(1, 10).first;

    // 4. Turn flash on and measure voltage through both sensors
    set_laser(0, true);
    set_laser(1, true);
    set_flash_level(35); // Flash at 35% generates normal 350mA current
    set_flash(true);
    task_sleep_ms(1);
    post_result.ambient_flash[0] = measure_avg_voltage(0, 1).first;
    set_flash(false);
    task_sleep_ms(20);
    set_flash(true);
    task_sleep_ms(1);
    post_result.ambient_flash[1] = measure_avg_voltage(1, 1).first;
    set_flash(false);

    unsigned int n; 
    for (n = 0; n < 2; n++) {
        post_result.ambient_single[n] -= post_result.dark_v[n];
        post_result.ambient_both[n] -= post_result.dark_v[n];
        post_result.ambient_flash[n] -= post_result.dark_v[n];
    }

    // 5.1 Test sensor 1 ref
    set_laser(0, true);
    set_laser(1, true);

    set_ref_voltage(0, post_result.ambient_both[0] + post_result.dark_v[0] - trigger_tolerance);
    task_sleep_ms(100);
    post_result.trig_high[0] = gpio_get(IO_S1_TRIG) == true;

    set_ref_voltage(0, post_result.ambient_both[0] + post_result.dark_v[0] + trigger_tolerance);
    task_sleep_ms(100);
    post_result.trig_low[0] = gpio_get(IO_S1_TRIG) == false;

    // 5.2 Test sensor 2 ref
    set_ref_voltage(1, post_result.ambient_both[1] + post_result.dark_v[1] - trigger_tolerance);
    task_sleep_ms(100);
    post_result.trig_high[1] = gpio_get(IO_S2_TRIG) == true;

    set_ref_voltage(1, post_result.ambient_both[1] + post_result.dark_v[1] + trigger_tolerance);
    task_sleep_ms(100);
    post_result.trig_low[1] = gpio_get(IO_S2_TRIG) == false;


    // Set ref voltages
    post_result.ref_level[0] = post_result.ambient_both[0] + post_result.dark_v[0] + trigger_tolerance;
    post_result.ref_level[1] = post_result.ambient_both[1] + post_result.dark_v[1] + trigger_tolerance;
    set_ref_voltage(0, post_result.ref_level[0]);
    set_ref_voltage(1, post_result.ref_level[1]);

    bool pass{true};
    for (n = 0; n < 2; n++) {
        pass &= post_result.trig_high[n];
        pass &= post_result.trig_low[n];
    }
    post_result.pass = pass;
    return pass;
}

const post_result_t &get_post_result() {
    return post_result;
}

void print_post_result(const post_result_t &result) {
    unsigned int n; 
    
    for (n = 0; n < 2; n++) {
        cli_info("S%d_TRIG_HIGH %s", n+1, result.trig_high[n] ? "PASS" : "FAIL");
        cli_info("S%d_TRIG_LOW %s", n+1, result.trig_low[n] ? "PASS" : "FAIL");
    }
    for (n = 0; n < 2; n++)
        cli_info("dark%d %.3fV", n+1, result.dark_v[n]);
    for (n = 0; n < 2; n++)
        cli_info("ambient_self%d %.3fV", n+1, result.ambient_single[n]);
    for (n = 0; n < 2; n++)
        cli_info("ambient_both%d %.3fV", n+1, result.ambient_both[n]);
    cli_info("ambient_cross2to1 %.3fV", result.ambient_both[0] - result.ambient_single[0]);
    cli_info("ambient_cross1to2 %.3fV", result.ambient_both[1] - result.ambient_single[1]);
    cli_info("flashto1 %.3fV", result.ambient_flash[0] - result.ambient_both[0]);
    cli_info("flashto2 %.3fV", result.ambient_flash[1] - result.ambient_both[1]);
    cli_info("ref_v1 %.3fV", result.ref_level[0]);
    cli_info("ref_v2 %.3fV", result.ref_level[1]);
    cli_info("POST %s", result.pass ? "PASS" : "FAIL");
}

