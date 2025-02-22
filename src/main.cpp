#include <Arduino.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>

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
#include "filter.h"
#include "correlator.h"
#include "utils.h"
#include "detector.h"
#include "post.h"
#include "benchmark.h"
#include "signal_chain.h"

// https://wiki.segger.com/How_to_debug_Arduino_a_Sketch_with_Ozone_and_J-Link


#define DEF_STACK_SIZE 1024



static uint16_t adc_channel_offset[2]{0};

static volatile float avg_voltage[2] = {-1., -1.};


cli_result_t analog_cmd(size_t argc, const char *argv[]) {
    auto s1 = measure_avg_voltage(0, 10);
    auto s2 = measure_avg_voltage(1, 10);

    double ref1 = get_ref_voltage(0);
    double ref2 = get_ref_voltage(1);

    cli_info("S1-IN  %.3fV", s1.first);
    cli_info("S2-IN  %.3fV", s2.first);
    cli_info("S1-NOISE %.3fmVrms", s1.second*1000.0);
    cli_info("S2-NOISE %.3fmVrms", s2.second*1000.0);
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

cli_result_t results_cmd(size_t argc, const char *argv[]) {
    int n = 0;
    auto q = get_detector_results_q();

    while (true) {
        detector::detected_object_t r;

        if (xQueueReceive(q, &r, 0) != pdPASS)
            break;
        cli_debug("robj(source=%d, start=%llu, len=%lu, ampl=%d, power=%d)",
            r.source, r.start, r.len, r.ampl, r.power);
        n++;
    }    
    cli_info("new_results %d", n);

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
    task_sleep_ms(time_ms);
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
        task_sleep_ms(time_ms);
        set_flash(false);
        task_sleep_ms(interval_ms - time_ms);
    }

    return CMD_OK;
}



cli_result_t post_cmd(size_t argc, const char *argv[]) {
    if (argc > 1)
        return CMD_ERROR;
    if (argc == 1) {
        if (!strcmp(argv[0], "run"))
            post();
        else
            return CMD_ERROR;
    }

    print_post_result(get_post_result());
    return CMD_OK;
}



cli_result_t test_cmd(size_t argc, const char *argv[]);

static const cli_cmd_t command_list[] = {
    {analog_cmd, "analog"},
    {digital_cmd, "digital"},
    {status_cmd, "status"},
    {led_cmd, "led"},
    {avg_v_cmd, "avgv"},
    {sensor_ref_cmd, "ref"},
    {fan_speed_cmd, "fan"},
    {flash_level_cmd, "flashlvl"},
    {flash_cmd, "flash"},
    {laser_cmd, "laser"},
    {post_cmd, "post"},
    {test_cmd, "test"},
    {flash_strobe_test_cmd, "flashstrobetest"},
    {benchmark_cmd, "b"},
    {results_cmd, "res"}
};

static int command_num = sizeof(command_list) / sizeof(command_list[0]);


extern int adc_offset;
cli_result_t test_cmd(size_t argc, const char *argv[]) {
    
    if (argc == 1) {
        if (strcmp(argv[0], "cin") == 0) {
            auto tap = get_circ_buf_tap();
            tap->trigger();
            int factor_a = 0;
            int factor_b = 0;
            size_t n;
            size_t ms{16};

            while (true) {
                circular_buf_tap_t data;
                
                if (tap->receive(&data, 10)) {
                    if (data.source == 0) {
                        // length is 700ms
                        if ((data.index > 300*ms) && (data.index < 600*ms))
                            for (n = 0; n < 16; n++) 
                                if (data.data[n] > 250)
                                    factor_a++;
                    } else {
                        for (n = 0; n < 16; n++) 
                            if (data.data[n] > 250)
                                factor_b++;

                    }

                    cli_info("%s # %s[%d]", 
                          format_vec(data.data, 16, "%4hu, ").c_str(),
                          data.source ? "b" : "a", (int)data.index);
                } else {
                    if (tap->is_done())
                        break;
                }
            }
            cli_info("peaks[a] %d", factor_a);
            cli_info("peaks[b] %d", factor_b);
        } else {
            cli_info("Unknown subcommand");
        }
        return CMD_OK;
    }
    const auto stat = get_signal_chain_stat();
    cli_info("adc_offset %d", adc_offset);
    cli_info("dma_samples %d", stat->dma_samples);
    cli_info("consumer_samples %d", stat->consumer_samples);
    cli_info("correlator_runs %d", stat->correlator_runs);
    cli_info("filter_in[0] %d", stat->filter_in[0]);
    cli_info("filter_in[1] %d", stat->filter_in[1]);
    cli_info("filter_out[0] %d", stat->filter_out[0]);
    cli_info("filter_out[1] %d", stat->filter_out[1]);
    cli_info("correlator_runtime %d", stat->correlator_runtime);

    return CMD_OK;
}

void heartbeat_task(void *pvParameters) {
    while (1) {
        set_led(0, true);
        task_sleep_ms(150);
        set_led(0, false);
        task_sleep_ms(150);
        set_led(0, true);
        task_sleep_ms(150);
        set_led(0, false);
        task_sleep_ms(550);
    }
}



void setup() {
    Serial.begin(115200);
    stdio_setup();
    cli_init(command_list, command_num);

    Wire.begin(); 
    gpio_begin();
    set_led(1, true);
    init_ref_pwm();
    init_fan();
    init_flash();
    init_signal_chain();

    TaskHandle_t t_heartbeat, t_analog, t_correlator;
    xTaskCreate(heartbeat_task, "heartbeat", 128, NULL, 1, &t_heartbeat);
    xTaskCreate(analog_task, "analog", DEF_STACK_SIZE, NULL, 3, &t_analog);
    xTaskCreate(correlator_task, "correlator", 2048, NULL, 2, &t_correlator);

    // configure tasks to run on core 1, but correlator on core 2 
    vTaskCoreAffinitySet(t_heartbeat, 0x1);
    vTaskCoreAffinitySet(t_analog, 0x1);
    vTaskCoreAffinitySet(t_correlator, 0x2);

    adc_begin();

    post();
    set_fan(true);
}

void loop() {
    if (Serial.available() < 1)
        task_sleep_ms(2);
    cli_loop();
}
