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

// https://wiki.segger.com/How_to_debug_Arduino_a_Sketch_with_Ozone_and_J-Link


#define DEF_STACK_SIZE 1024
void filter_benchmark(size_t rounds, int stage);
void filter_benchmark_cic_cpp(size_t rounds, int stage);
void filter_benchmark_cic_c(size_t rounds, int stage);
void filter_benchmark_fir(size_t rounds, int stage);
void correlator_benchmark(size_t rounds, int stage);

typedef struct {
    int offset;
    float peak;
} correlator_result_t;

typedef struct {
    uint8_t source;
    uint16_t index;
    uint16_t data[16];
} circular_buf_tap_t;

typedef struct {
    uint16_t index;
    float peak;
} correlator_tap_t;


static uint16_t adc_channel_offset[2]{0};
static volatile uint32_t n_dma_samples{0};
static volatile uint32_t n_consumer_samples{0};
static volatile uint32_t n_correlator_runs{0};
static volatile float avg_voltage[2] = {-1., -1.};
static volatile uint32_t filter_in[2] = {0};
static volatile uint32_t filter_out[2] = {0};
static volatile int32_t  correlator_offset{0};
static volatile uint32_t correlator_runtime{0};
QueueHandle_t correlator_results_q{nullptr};
static std::shared_ptr<data_queue::DataTap<circular_buf_tap_t>> circ_buf_tap{nullptr};
static std::shared_ptr<data_queue::DataTap<correlator_tap_t>> correlator_tap{nullptr};

cli_result_t analog_cmd(size_t argc, const char *argv[]) {
    adc_disable_dma();
    task_sleep_ms(10);

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

cli_result_t results_cmd(size_t argc, const char *argv[]) {
    int n = 0;
    while (true) {
        correlator_result_t r;

        if (xQueueReceive(correlator_results_q, &r, 0) != pdPASS)
            break;
        cli_debug("Delay %d ampl %f", r.offset, r.peak);
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


void post() {
    float dark_v[2];
    float ambient_single[2];
    float ambient_both[2];
    float ambient_flash[2];

    // 1. Turn the fan off, laser off and measure dark voltage on sensors
    set_fan(false);
    task_sleep_ms(500);
    set_laser(0, false);
    set_laser(1, false);
    task_sleep_ms(20);
    dark_v[0] = measure_avg_voltage(0, 20).first;
    dark_v[1] = measure_avg_voltage(1, 20).first;

    // 2.1. Turn the laser1 on and measure ambient light through sensor1
    set_laser(0, true);
    set_laser(1, false);
    task_sleep_ms(20);
    ambient_single[0] = measure_avg_voltage(0, 20).first;

    // 2.1. Turn the laser2 on and measure ambient light through sensor2
    set_laser(0, false);
    set_laser(1, true);
    task_sleep_ms(20);
    ambient_single[1] = measure_avg_voltage(1, 20).first;

    // 3.1. Turn both lasers on and measure ambient light through both sensors
    set_laser(0, true);
    set_laser(1, true);
    task_sleep_ms(20);
    ambient_both[0] = measure_avg_voltage(0, 20).first;
    ambient_both[1] = measure_avg_voltage(1, 20).first;

    // 4. Turn flash on and measure voltage through both sensors
    set_laser(0, true);
    set_laser(1, true);
    set_flash_level(35); // Flash at 35% generates normal 350mA current
    set_flash(true);
    task_sleep_ms(1);
    ambient_flash[0] = measure_avg_voltage(0, 1).first;
    set_flash(false);
    task_sleep_ms(20);
    set_flash(true);
    task_sleep_ms(1);
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
    task_sleep_ms(50);
    pass = gpio_get(IO_S1_TRIG) == true;
    cli_info("S1_TRIG_HIGH %s", pass ? "PASS" : "FAIL");

    set_ref_voltage(0, ambient_both[0] + dark_v[0] + 0.02);
    task_sleep_ms(50);
    pass = gpio_get(IO_S1_TRIG) == false;
    cli_info("S1_TRIG_LOW %s", pass ? "PASS" : "FAIL");

    // 5.2 Test sensor 2 ref
    set_ref_voltage(1, ambient_both[1] + dark_v[1] - 0.02);
    task_sleep_ms(50);
    pass = gpio_get(IO_S2_TRIG) == true;
    cli_info("S2_TRIG_HIGH %s", pass ? "PASS" : "FAIL");

    set_ref_voltage(1, ambient_both[1] + dark_v[1] + 0.02);
    task_sleep_ms(50);
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

    adc_channel_offset[0] = adc_V_to_raw(dark_v[0]); 
    adc_channel_offset[1] = adc_V_to_raw(dark_v[1]); 
}

cli_result_t post_cmd(size_t argc, const char *argv[]) {

    post();
    return CMD_OK;
}

cli_result_t benchmark_cmd(size_t argc, const char *argv[]) {
    size_t n_rounds = 2000;
    int stage = 0;
    size_t samples_per_round = 128;
    void (* benchmark_func)(size_t n_rounds, int state) = filter_benchmark;
    const char *benchmark_name = "total";

    if (argc >= 1) {
        if (argc > 1) 
            stage = atoi(argv[1]);
    
        if (!strcmp(argv[0], "cic")) {
            benchmark_func = filter_benchmark_cic_cpp;
            benchmark_name = "CIC/C++";
        } else
        if (!strcmp(argv[0], "cicc")) {
            benchmark_func = filter_benchmark_cic_c;
            benchmark_name = "CIC/C";
        } else
        if (!strcmp(argv[0], "fir")) {
            benchmark_func = filter_benchmark_fir;
            benchmark_name = "FIR";
        } else
        if (!strcmp(argv[0], "cor")) {
            benchmark_func = correlator_benchmark;
            n_rounds = 1;
            samples_per_round = 1;
            benchmark_name = "COR";
        } else
        if (!strcmp(argv[0], "fir")) {
            benchmark_func = filter_benchmark;
            benchmark_name = "total";
        }
    }

    TickType_t start = xTaskGetTickCount();
    benchmark_func(n_rounds, stage);
    TickType_t stop = xTaskGetTickCount();

    size_t time_ms = (stop - start) * portTICK_PERIOD_MS;

    cli_info("benchmark %s", benchmark_name);
    cli_info("stage %d", stage);
    cli_info("rounds %d", n_rounds);
    cli_info("time,ms %d", time_ms);
    cli_info("samples %d", n_rounds * samples_per_round);
    cli_info("samples/s %d", (n_rounds * samples_per_round * 1000) / time_ms );

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
            circ_buf_tap->trigger();
            int factor_a = 0;
            int factor_b = 0;
            size_t n;
            size_t ms{16};

            while (true) {
                circular_buf_tap_t data;
                
                if (circ_buf_tap->receive(&data, 10)) {
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
                    if (circ_buf_tap->is_done())
                        break;
                }
            }
            cli_info("peaks[a] %d", factor_a);
            cli_info("peaks[b] %d", factor_b);
        } else if (strcmp(argv[0], "cout") == 0) {
            correlator_tap->trigger();
            while (true) {
                correlator_tap_t data;
                if (correlator_tap->receive(&data, 2000)) {
                    cli_info("corr[%d] %f", (int)data.index, data.peak);
                } else {
                    if (correlator_tap->is_done())
                        break;
                }
            }
        } else {
            cli_info("Unknown subcommand");
        }
        return CMD_OK;
    }

    cli_info("adc_offset %d", adc_offset);
    cli_info("dma_samples %d", n_dma_samples);
    cli_info("consumer_samples %d", n_consumer_samples);
    cli_info("correlator_runs %d", n_correlator_runs);
    cli_info("filter_in[0] %d", filter_in[0]);
    cli_info("filter_in[1] %d", filter_in[1]);
    cli_info("filter_out[0] %d", filter_out[0]);
    cli_info("filter_out[1] %d", filter_out[1]);
    // cli_info("Vavg[0] %f", avg_voltage[0]);
    // cli_info("Vavg[1] %f", avg_voltage[1]);
    cli_info("correlator_offset %d", correlator_offset);
    cli_info("correlator_runtime %d", correlator_runtime);

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


// FIR low pass filter 
// Fs 48kHz, 
// Passband 5000Hz
// Stopband 8000Hz
// Stopband attenuation 75dB
// length = 47
// Fout 16kHz after decimation=3 
static std::vector<float> fir_lp_48k_5k
{
     -0.000393, -0.000876, -0.001113, -0.000412,
     0.001640, 0.004554, 0.006716, 0.006048,
     0.001485, -0.005606, -0.011269, -0.010823,
     -0.002123, 0.011747, 0.022629, 0.021264,
     0.003557, -0.024493, -0.046941, -0.044427,
     -0.004546, 0.069449, 0.157022, 0.227790,
     0.254931, 0.227790, 0.157022, 0.069449,
     -0.004546, -0.044427, -0.046941, -0.024493,
     0.003557, 0.021264, 0.022629, 0.011747,
     -0.002123, -0.010823, -0.011269, -0.005606,
     0.001485, 0.006048, 0.006716, 0.004554,
     0.001640, -0.000412, -0.001113, -0.000876,
     -0.000393,
};

std::shared_ptr<data_queue::QueuedDataConsumer> sample_queue{nullptr};

void analog_task(void *pvParameters) {
    auto consumer = std::make_shared<queued_adc::QueuedADCConsumer>();
    data_queue::data_queue_msg_t *data_sink{nullptr};
    size_t data_sink_fill = 0;
    adc_dma_config_t default_dma_config = {
        n_inputs: 2,
        inputs: {ADC_CH_S1, ADC_CH_S2}, 
        sample_freq: 500000,
        consumer: consumer
    };
    
    // First-stage lowpass filters with passband < 50kHz and decimation = 5
    // Has output rate of 50ksps
    filter::CICFilter</* M */4, /* R */5> filter_first_a{};
    filter::CICFilter</* M */4, /* R */5> filter_first_b{};

    // Second-stage lowpass filters with passband 5kHz and decimation = 3
    // Has output rate of 16ksps
    filter::FIRFilter filter_second_a(fir_lp_48k_5k, 3);
    filter::FIRFilter filter_second_b(fir_lp_48k_5k, 3);

    adc_set_default_dma(&default_dma_config);

    float cic_gain{filter_first_a.gain()};

    while (true) {
        const queued_adc::adc_queue_msg_t *msg;
        constexpr size_t out_buf_len{ADC_BUF_LEN/2};
        size_t len;
        uint16_t out_buf[out_buf_len];

        // receive ADC data buffer
        n_dma_samples++;
        msg = consumer->receive_msg(10);
        
        if (!msg) // should not happen
            continue;

        // .. process incoming stage
        // here it's for test only
        filter_in[0] += ADC_BUF_LEN/2;
        filter_in[1] += ADC_BUF_LEN/2;

        filter_first_a.write(msg->buffer, ADC_BUF_LEN, 2, -adc_channel_offset[0]);
        filter_first_b.write(msg->buffer + 1, ADC_BUF_LEN, 2, -adc_channel_offset[1]);

        // return ADC data buffer
        consumer->return_msg(msg);

        // try to obtain sink buffer if it isn't
        if (!data_sink && (sample_queue != nullptr)) {
            data_sink = sample_queue->send_msg_claim();
            data_sink_fill = 0;
        }

        // .. process second stage
        if (filter_first_a.out_len() > 16) {
            len = filter_first_a.read(out_buf, out_buf_len);
            if (len > 0)
                filter_second_a.write(out_buf, len);
        }

        if (filter_first_b.out_len() > 16) {
            len = filter_first_b.read(out_buf, out_buf_len);
            if (len > 0)
                filter_second_b.write(out_buf, len);
        }


        // .. consume second stage results
        size_t second_fill{std::min(filter_second_a.out_len(), filter_second_b.out_len())};
        if ((data_sink != nullptr) && (second_fill >= 16)) {
            size_t max_read{data_queue::DATA_BUF_LEN - data_sink_fill};
            size_t to_read{std::min(second_fill, max_read)};
            auto len_a = filter_second_a.read(data_sink->buffer_a + data_sink_fill, to_read);
            auto len_b = filter_second_b.read(data_sink->buffer_b + data_sink_fill, to_read);
            data_sink_fill += std::min(len_a, len_b); // Normally they should be equal
            if (data_sink_fill == data_queue::DATA_BUF_LEN) {
                // Send message
                sample_queue->send_msg_send(data_sink);
                // Get another message or nullptr
                data_sink = sample_queue->send_msg_claim();
                data_sink_fill = 0;
            }
        }
    }
}

void correlator_task(void *pvParameters) {
    sample_queue = std::make_shared<data_queue::QueuedDataConsumer>();
    constexpr unsigned int fs{16000};
    constexpr unsigned int ms{fs/1000};

    constexpr unsigned int processing_interval{1000*ms};

    constexpr unsigned int offset_a_min{100*ms};
    constexpr unsigned int offset_a_max{400*ms};

    correlator::CircularBuffer buf_a(700*ms);
    correlator::CircularBuffer buf_b(200*ms);
    size_t data_cnt{0};
    
    while (true) {
        // receive ADC data buffer
        const auto msg = sample_queue->receive_msg();
        n_consumer_samples++;
    
        constexpr auto rx_len{data_queue::DATA_BUF_LEN};
        buf_a.write(msg->buffer_a, rx_len);
        buf_b.write(msg->buffer_b, rx_len);
        data_cnt += rx_len;
        sample_queue->receive_msg_return(msg);
    

        if (circ_buf_tap->is_triggered()) {
            circular_buf_tap_t tap;
            tap.source = 0;
            int size = buf_a.get_capacity();
            auto data =  buf_a.get_data();
            int pos = buf_a.get_data_ptr();
            for (int n = 0; n < buf_a.get_capacity(); n += ms) {
                tap.index = n;
                for (int m = 0; m < ms; m++) {
                    int idx = (pos + n + m) % size;
                    tap.data[m] = data[idx];
                }
                circ_buf_tap->send(tap);
            }
            tap.source = 1;
            size = buf_b.get_capacity();
            data =  buf_b.get_data();
            pos = buf_b.get_data_ptr();
            for (int n = 0; n < buf_b.get_capacity(); n += ms) {
                tap.index = n;
                for (int m = 0; m < ms; m++) {
                    int idx = (pos + n + m) % size;
                    tap.data[m] = data[idx];
                }
                circ_buf_tap->send(tap);
            }
            circ_buf_tap->complete();
        }

        // Do processing
        if (data_cnt >= processing_interval) {
            data_cnt = 0;

            n_correlator_runs++;

            TickType_t start = xTaskGetTickCount();

            int32_t max_index{0};
            uint64_t max_val{0};
            static uint64_t bins[(offset_a_max - offset_a_min)/(5*ms)];
            memset(bins, 0, sizeof(bins));
            constexpr size_t bins_size{sizeof(bins)/sizeof(bins[0])};

            correlator::correlate_callback_t f([&](int32_t offset, uint64_t sum) {
                bins[(offset - offset_a_min)/(5*ms)] += sum;
                if (sum > max_val) {
                    max_val = sum;
                    max_index = offset;
                }
            }); 

            correlator::correlate(buf_a, buf_b, offset_a_min, offset_a_max, buf_b.get_capacity(), f);

            TickType_t stop = xTaskGetTickCount();

            correlator_runtime = (stop - start) * portTICK_PERIOD_MS; // ms
            correlator_offset  = max_index / ms;

            correlator_result_t res;
            res.offset = max_index / ms;
            res.peak = max_val;
            xQueueSendToBack(correlator_results_q, &res, 0);

            if (correlator_tap->is_triggered()) {
                correlator_tap_t tap;
                for (size_t m = 0; m < bins_size; m++) {
                    tap.index = (offset_a_min/ms) + m * 5;
                    tap.peak = (float)bins[m];
                    correlator_tap->send(tap);
                }
                correlator_tap->complete();
            }
        }
    }
}


// baseline
// benchmark 0
// [I] rounds 2000
// [I] time,ms 441
// [I] samples 256000
// [I] samples/s 580498
// benchmark 1
// [I] rounds 2000
// [I] time,ms 229
// [I] samples 256000
// [I] samples/s 1117903


// current result
// [I] benchmark 0
// [I] rounds 2000
// [I] time,ms 182
// [I] samples 256000
// [I] samples/s 1406593
// [I] benchmark 1
// [I] rounds 2000
// [I] time,ms 46
// [I] samples 256000
// [I] samples/s 5565217



void filter_benchmark_cic_cpp(size_t rounds, int stage) {
    filter::CICFilter</* M */4, /* R */5> filter_first;

    uint16_t rx_buf[ADC_BUF_LEN];
    constexpr size_t out_buf_len{ADC_BUF_LEN};
    uint16_t out_buf[out_buf_len];

    memset(rx_buf, 0, sizeof(rx_buf));
    rx_buf[0] = 1000;
    rx_buf[1] = 1000;
    rx_buf[32] = 1000;
    rx_buf[33] = 1000;
    rx_buf[64] = 1000;
    rx_buf[65] = 1000;

    while (rounds--) {
        size_t len;

        // receive ADC data buffer
        n_dma_samples++;

        // .. process incoming stage

        filter_first.write(rx_buf, ADC_BUF_LEN);
        if (stage == 1)
            continue;

        // .. process second stage
        if (filter_first.out_len() > 64) {
            len = filter_first.read(out_buf, out_buf_len);
        }
    }
}

void filter_benchmark_cic_c(size_t rounds, int stage) {
    cic_filter_t filter_first;
    cic_init(&filter_first, /* M */4, /* R */5);

    uint16_t rx_buf[ADC_BUF_LEN];
    constexpr size_t out_buf_len{ADC_BUF_LEN};
    uint16_t out_buf[out_buf_len];

    memset(rx_buf, 0, sizeof(rx_buf));
    rx_buf[0] = 1000;
    rx_buf[1] = 1000;
    rx_buf[32] = 1000;
    rx_buf[33] = 1000;
    rx_buf[64] = 1000;
    rx_buf[65] = 1000;

    while (rounds--) {
        size_t len;

        // receive ADC data buffer
        n_dma_samples++;

        // .. process incoming stage

        cic_write<4,5>(&filter_first, rx_buf, ADC_BUF_LEN, 1);
        if (stage == 1)
            continue;

        // .. process second stage
        if (filter_first.out_cnt > 64) {
            len = cic_read(&filter_first, out_buf, out_buf_len);
        }
    }
}

// baseline
// [I] benchmark FIR
// [I] stage 0
// [I] rounds 2000
// [I] time,ms 325
// [I] samples 256000
// [I] samples/s 787692
// [I] benchmark FIR
// [I] stage 1
// [I] rounds 2000
// [I] time,ms 312
// [I] samples 256000
// [I] samples/s 820512

// results
// [I] benchmark FIR
// [I] stage 0
// [I] rounds 2000
// [I] time,ms 217
// [I] samples 256000
// [I] samples/s 1179723
// [I] benchmark FIR
// [I] stage 1
// [I] rounds 2000
// [I] time,ms 211
// [I] samples 256000
// [I] samples/s 1213270

void filter_benchmark_fir(size_t rounds, int stage) {
    // Second-stage lowpass filters with passband 5kHz and decimation = 3
    // Has output rate of 16ksps
    filter::FIRFilter filter_second(fir_lp_48k_5k, 3);
    
    constexpr size_t out_buf_len{ADC_BUF_LEN};
    uint16_t out_buf[out_buf_len];
    uint16_t rx_buf[64];

    memset(rx_buf, 0, sizeof(rx_buf));
    rx_buf[0] = 1000;
    rx_buf[1] = 1000;
    rx_buf[32] = 1000;
    rx_buf[33] = 1000;

    while (rounds--) {
        filter_second.write(rx_buf, 64);

        if (filter_second.out_len() > 64) {
            if (stage == 1)
                filter_second.out_flush();
            else
                filter_second.read(out_buf, out_buf_len);
        }
    }
}


void correlator_benchmark(size_t rounds, int stage) {
    constexpr unsigned int fs{16000};
    constexpr unsigned int ms{fs/1000};

    constexpr unsigned int offset_a_min{200*ms};
    constexpr unsigned int offset_a_max{350*ms};
    constexpr unsigned int buf_fill{700*ms};

    correlator::CircularBuffer buf_a(buf_fill); // 0.7s
    correlator::CircularBuffer buf_b(300*ms);  // 0.3s
    uint16_t buf1[16];
    uint16_t buf2[16];
    for (unsigned int n = 0; n < 16; n ++) {
        buf1[n] = 100;
        buf2[n] = 100;
    }
    buf2[3] = 125;
    buf2[4] = 150;
    buf2[5] = 700;
    buf2[6] = 150;
    buf2[7] = 125;
    constexpr unsigned int a_offset{buf_fill - 300*ms};
    constexpr unsigned int b_offset{buf_fill - 20*ms};

    // fill buffers with same values and a chirp positioned
    // at buf_a[-300ms], buf_b[-20ms]
    for (unsigned int n = 0; n < buf_fill; n += 16) {
        if (n == a_offset)
            buf_a.write(buf2, 16);
        else
            buf_a.write(buf1, 16);

        if (n == b_offset)
            buf_b.write(buf2, 16);
        else
            buf_b.write(buf1, 16);
    }

    while (rounds--) {
        auto ret = correlator::correlate_max(buf_a, buf_b, offset_a_min, offset_a_max, buf_b.get_capacity());
        correlator_offset  = ret.first / ms;
    }
}

//#define CIC_C 1
void filter_benchmark(size_t rounds, int stage) {
    // First-stage lowpass filters with passband < 50kHz and decimation = 5
    // Has output rate of 50ksps
#ifndef CIC_C
    filter::CICFilter</* M */4, /* R */5> filter_first;
#else
    cic_filter_t filter_first;
    cic_init(&filter_first, /* M */4, /* R */5);
#endif

    // Second-stage lowpass filters with passband 5kHz and decimation = 3
    // Has output rate of 16ksps
    filter::FIRFilter filter_second(fir_lp_48k_5k, 3);

    uint16_t rx_buf[ADC_BUF_LEN];
    constexpr size_t out_buf_len{ADC_BUF_LEN};
    uint16_t out_buf[out_buf_len];

    memset(rx_buf, 0, sizeof(rx_buf));
    rx_buf[0] = 1000;
    rx_buf[1] = 1000;
    rx_buf[32] = 1000;
    rx_buf[33] = 1000;
    rx_buf[64] = 1000;
    rx_buf[65] = 1000;

    while (rounds--) {
        size_t len;

        // receive ADC data buffer
        n_dma_samples++;

        // .. process incoming stage

#ifndef CIC_C
        filter_first.write(rx_buf, ADC_BUF_LEN);
#else
        cic_write<4,5>(&filter_first, rx_buf, ADC_BUF_LEN, 1);
#endif
        if (stage == 1)
            continue;

        // .. process second stage
#ifndef CIC_C
        if (filter_first.out_len() > 64) {
            len = filter_first.read(out_buf, out_buf_len);
#else
        if (filter_first.out_cnt > 64) {
            len = cic_read(&filter_first, out_buf, out_buf_len);
#endif
            if (stage == 2)
                continue;
            if (len > 0)
                filter_second.write(out_buf, len);
        }

        // .. consume second stage results
        if (filter_second.out_len() > 64) {
            int avg{0};
            if (stage == 3)
                filter_second.out_flush();
            else
                filter_second.read(out_buf, out_buf_len);
        }
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

    correlator_results_q = xQueueCreate(128, sizeof(correlator_result_t));
    circ_buf_tap = std::make_shared<data_queue::DataTap<circular_buf_tap_t>>();
    correlator_tap = std::make_shared<data_queue::DataTap<correlator_tap_t>>();


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
