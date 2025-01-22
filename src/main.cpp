#include <Arduino.h>
#include <stdio.h>

#include "board_def.h"
#include "io.h"
#include <Wire.h> // I2C library
#include "adc.h"
#include "fan.h"
#include "flash.h"
#include "ref_pwm.h"
#include "stdio_hooks.h"
#include "cli.h"
#include "cli_out.h"
#include "analog.h"

// https://wiki.segger.com/How_to_debug_Arduino_a_Sketch_with_Ozone_and_J-Link



static uint32_t n_dma_samples = 0;


cli_result_t analog_cmd(size_t argc, const char *argv[]) {
    adc_disable_dma();
    sleep_ms(10);

    double s1 = adc_read_V(ADC_CH_S1);
    double s2 = adc_read_V(ADC_CH_S2);

    adc_enable_dma();

    double ref1 = get_ref_voltage(0);
    double ref2 = get_ref_voltage(1);

    cli_info("S1-IN  %.3fV", s1);
    cli_info("S2-IN  %.3fV", s2);
    cli_info("S1-REF %.3fV", ref1);
    cli_info("S2-REF %.3fV", ref2);
    cli_info("FLASH-PWR %d%%", (int)get_flash_level());
    cli_info("FAN-SPEED %d%%", (int)get_fan_speed());
    
    return CMD_OK;
}

cli_result_t digital_cmd(size_t argc, const char *argv[]) {
    cli_info("CAM0 %d", gpio_get(IO_CAM0));
    cli_info("CAM1 %d", gpio_get(IO_CAM1));
    cli_info("CAM2 %d", gpio_get(IO_CAM2));
    cli_info("CAM3 %d", gpio_get(IO_CAM3));
    cli_info("AUX0 %d", gpio_get(IO_AUX0));
    cli_info("AUX1 %d", gpio_get(IO_AUX1));
    cli_info("AUX2 %d", gpio_get(IO_AUX2));
    cli_info("AUX3 %d", gpio_get(IO_AUX3));

    return CMD_OK;
}

cli_result_t status_cmd(size_t argc, const char *argv[]) {
    cli_info("LASER1  %d", gpio_get(IO_LASER1));
    cli_info("LASER2  %d", gpio_get(IO_LASER2));
    cli_info("FAN-PWR %d", gpio_get(IO_FAN_PWR_C));
    cli_info("FLASH   %d", gpio_get(IO_FL_ON));
    cli_info("LED1 %d", gpio_get(IO_LED1));
    cli_info("LED2 %d", gpio_get(IO_LED2));

    return CMD_OK;
}



cli_result_t led_cmd(size_t argc, const char *argv[]) {
    if (argc != 2)
        return CMD_ERROR;

    int led  = atoi(argv[0]);
    int mode = onoff_to_bool(argv[1]);
    
    if ((led < 0) || (led > 2))
        return CMD_ERROR;
    if (mode < 0)
        return CMD_ERROR;

    set_led(led, mode);
    return CMD_OK;
}


cli_result_t laser_cmd(size_t argc, const char *argv[]) {
    if (argc != 2)
        return CMD_ERROR;

    int laser  = atoi(argv[0]);
    int mode = onoff_to_bool(argv[1]);
    
    if ((laser < 0) || (laser > 1))
        return CMD_ERROR;
    if (mode < 0)
        return CMD_ERROR;

    set_laser(laser, mode);
    return CMD_OK;
}


cli_result_t avg_v_cmd(size_t argc, const char *argv[]) {
    if (argc != 2)
        return CMD_ERROR;

    int ch = atoi(argv[0]);
    int ms = atoi(argv[1]);
    if ((ch < 0) || (ch > 2))
        return CMD_ERROR;
    if (ms <= 0)
        return CMD_ERROR;

    auto val = measure_avg_voltage(ch, ms);
    cli_info("AVG[%d] %.3fV", ch, val.first);
    cli_info("NOISE[%d] %.6fVrms", ch, val.second);
    return CMD_OK;
}

cli_result_t sensor_ref_cmd(size_t argc, const char *argv[]) {
    if (argc != 2)
        return CMD_ERROR;

    int ch = atoi(argv[0]);
    int mv = atoi(argv[1]);
    if ((ch < 0) || (ch > 1))
        return CMD_ERROR;
    if ((mv < 0) || (mv > 3300))
        return CMD_ERROR;

    set_ref_voltage(ch, mv / 1000.0f);

    double v = get_ref_voltage(ch);
    cli_info("S%d-REF %.3fV", ch + 1, v);
    return CMD_OK;
}

cli_result_t fan_speed_cmd(size_t argc, const char *argv[]) {
    if ((argc != 1) && (argc != 2))
        return CMD_ERROR;

    bool mode = onoff_to_bool(argv[0]);
    if (argc == 2) {
        int dc = atoi(argv[1]);
        if ((dc < 0) || (dc > 100))
            return CMD_ERROR;

        set_fan_speed(dc);
        
        int fan = get_fan_speed();
        cli_info("FAN-SPEED %d%%", fan);
    }
    set_fan(mode);
    cli_info("FAN-PWR %d", mode);
    return CMD_OK;
}



cli_result_t flash_level_cmd(size_t argc, const char *argv[]) {
    if (argc != 1)
        return CMD_ERROR;

    int dc = atoi(argv[0]);
    if ((dc < 0) || (dc > 100))
        return CMD_ERROR;

    set_flash_level(dc);
    
    cli_info("FLASH-PWR %d%%", (int)get_flash_level());
    return CMD_OK;
}

cli_result_t flash_cmd(size_t argc, const char *argv[]) {
    int time_ms = 5;
    
    if (argc == 1) {
        time_ms = atoi(argv[0]);
    } else if (argc == 0) {
        // use default time
    } else
        return CMD_ERROR;

    set_flash(true);
    sleep_ms(time_ms);
    set_flash(false);

    return CMD_OK;
}

cli_result_t flash_strobe_test_cmd(size_t argc, const char *argv[]) {
    int time_ms = 5;
    int interval_ms = 22; // 45 fps
    
    cli_debug("Starting flash test with time_on = %dms, fps = %d", time_ms, 1000/interval_ms);
    cli_debug("Reboot to cancel");
    cli_debug("Do not stare directly!");
    for (;;) {
        set_flash(true);
        sleep_ms(time_ms);
        set_flash(false);
        sleep_ms(interval_ms - time_ms);
    }

    return CMD_OK;
}


void post() {
    float dark_v[2];
    float ambient_single[2];
    float ambient_both[2];
    float ambient_flash[2];

    // 1. Turn the fan off, laser off and measure dark voltage on sensors
    set_fan(false);
    sleep_ms(500);
    set_laser(0, false);
    set_laser(1, false);
    sleep_ms(20);
    dark_v[0] = measure_avg_voltage(0, 20).first;
    dark_v[1] = measure_avg_voltage(1, 20).first;

    // 2.1. Turn the laser1 on and measure ambient light through sensor1
    set_laser(0, true);
    set_laser(1, false);
    sleep_ms(20);
    ambient_single[0] = measure_avg_voltage(0, 20).first;

    // 2.1. Turn the laser2 on and measure ambient light through sensor2
    set_laser(0, false);
    set_laser(1, true);
    sleep_ms(20);
    ambient_single[1] = measure_avg_voltage(1, 20).first;

    // 3.1. Turn both lasers on and measure ambient light through both sensors
    set_laser(0, true);
    set_laser(1, true);
    sleep_ms(20);
    ambient_both[0] = measure_avg_voltage(0, 20).first;
    ambient_both[1] = measure_avg_voltage(1, 20).first;

    // 4. Turn flash on and measure voltage through both sensors
    set_laser(0, true);
    set_laser(1, true);
    set_flash_level(35); // Flash at 35% generates normal 350mA current
    set_flash(true);
    sleep_ms(1);
    ambient_flash[0] = measure_avg_voltage(0, 1).first;
    set_flash(false);
    sleep_ms(20);
    set_flash(true);
    sleep_ms(1);
    ambient_flash[1] = measure_avg_voltage(1, 1).first;
    set_flash(false);

    unsigned int n; 
    for (n = 0; n < 2; n++) {
        ambient_single[n] -= dark_v[n];
        ambient_both[n] -= dark_v[n];
        ambient_flash[n] -= dark_v[n];
    }

    // 5.1 Test sensor 1 ref
    bool pass;
    set_laser(0, true);
    set_laser(1, true);

    set_ref_voltage(0, ambient_both[0] + dark_v[0] - 0.02);
    sleep_ms(50);
    pass = gpio_get(IO_S1_TRIG) == true;
    cli_info("S1_TRIG_HIGH %s", pass ? "PASS" : "FAIL");

    set_ref_voltage(0, ambient_both[0] + dark_v[0] + 0.02);
    sleep_ms(50);
    pass = gpio_get(IO_S1_TRIG) == false;
    cli_info("S1_TRIG_LOW %s", pass ? "PASS" : "FAIL");

    // 5.2 Test sensor 2 ref
    set_ref_voltage(1, ambient_both[1] + dark_v[1] - 0.02);
    sleep_ms(50);
    pass = gpio_get(IO_S2_TRIG) == true;
    cli_info("S2_TRIG_HIGH %s", pass ? "PASS" : "FAIL");

    set_ref_voltage(1, ambient_both[1] + dark_v[1] + 0.02);
    sleep_ms(50);
    pass = gpio_get(IO_S2_TRIG) == false;
    cli_info("S2_TRIG_LOW %s", pass ? "PASS" : "FAIL");


    for (n = 0; n < 2; n++)
        cli_info("dark%d %.3fV", n+1, dark_v[n]);
    for (n = 0; n < 2; n++)
        cli_info("ambient_self%d %.3fV", n+1, ambient_single[n]);
    for (n = 0; n < 2; n++)
        cli_info("ambient_both%d %.3fV", n+1, ambient_both[n]);
    cli_info("ambient_cross2to1 %.3fV", ambient_both[0] - ambient_single[0]);
    cli_info("ambient_cross1to2 %.3fV", ambient_both[1] - ambient_single[1]);
    cli_info("flashto1 %.3fV", ambient_flash[0] - ambient_both[0]);
    cli_info("flashto2 %.3fV", ambient_flash[1] - ambient_both[1]);

    // Set ref voltages
    set_ref_voltage(0, ambient_both[0] + dark_v[0] + 0.02);
    set_ref_voltage(1, ambient_both[1] + dark_v[1] + 0.03);
}

cli_result_t post_cmd(size_t argc, const char *argv[]) {

    post();
    return CMD_OK;
}

cli_result_t test_cmd(size_t argc, const char *argv[]);

static const cli_cmd_t command_list[] = {
    {analog_cmd, "analog"},
    {digital_cmd, "digital"},
    {status_cmd, "status"},
    {test_cmd, "test"},
    {led_cmd, "led"},
    {avg_v_cmd, "avgv"},
    {sensor_ref_cmd, "ref"},
    {fan_speed_cmd, "fan"},
    {flash_level_cmd, "flashlvl"},
    {flash_cmd, "flash"},
    {laser_cmd, "laser"},
    {post_cmd, "post"},
    {test_cmd, "test"},
    {flash_strobe_test_cmd, "flashstrobetest"}
};

static int command_num = sizeof(command_list) / sizeof(command_list[0]);

static adc_dma_config_t default_dma_config = {
    n_inputs: 2,
    inputs: {ADC_CH_S1, ADC_CH_S2}, 
    sample_freq: 250000,
    consumer: std::make_shared<DefaultADCConsumer>()
};

extern int adc_offset;
cli_result_t test_cmd(size_t argc, const char *argv[]) {

    if (argc == 1) {
        if (strcmp(argv[0], "dmadis") == 0) {
            adc_disable_dma();
            cli_info("Disabled DMA");
        } else if (strcmp(argv[0], "dmaen") == 0) {
            adc_enable_dma();
            cli_info("Enabled DMA");
        } else if (strcmp(argv[0], "defdma") == 0) {
            adc_set_default_dma(&default_dma_config);
            cli_info("Set default DMA");
        } else {
            cli_info("Unknown subcommand");
        }
        return CMD_OK;
    }

    cli_info("adc_offset %d", adc_offset);
    cli_info("dma samples %d", n_dma_samples);

    return CMD_OK;
}

void setup() {
    Serial.begin(115200);
    stdio_setup();
    cli_init(command_list, command_num);

    Wire.begin(); 
    gpio_begin();
    init_ref_pwm();
    init_fan();
    init_flash();
    set_led(1, false);

    adc_begin();

    adc_set_default_dma(&default_dma_config);

    post();
    set_fan(true);
}

void heartbeat() {
  unsigned int m = millis() % 1000;
  if (m < 200)
    set_led(0, true);
  else if (m < 400)
    set_led(0, false);
  else if (m < 600)
    set_led(0, true);
  else
    set_led(0, false);
}

void loop() {
  heartbeat();
  cli_loop();
}
