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

// https://wiki.segger.com/How_to_debug_Arduino_a_Sketch_with_Ozone_and_J-Link


#define DEF_STACK_SIZE 1024


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
QueueHandle_t detector_results_q{nullptr};
static std::shared_ptr<data_queue::DataTap<circular_buf_tap_t>> circ_buf_tap{nullptr};

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
    while (true) {
        detector::detected_object_t r;

        if (xQueueReceive(detector_results_q, &r, 0) != pdPASS)
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

        filter_first_a.write(msg->buffer, ADC_BUF_LEN, 2);
        filter_first_b.write(msg->buffer + 1, ADC_BUF_LEN, 2);

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

    // Circular buffers are for debug only
    correlator::CircularBuffer buf_a(500*ms);
    correlator::CircularBuffer buf_b(500*ms);
    size_t data_cnt{0};
    
    constexpr unsigned int len_threshold{10};
    constexpr unsigned int det_threshold{8};
    detector::ObjectDetector det_a(det_threshold, len_threshold);
    detector::ObjectDetector det_b(det_threshold, len_threshold);
    det_a.preinit(adc_channel_offset[0]);
    det_b.preinit(adc_channel_offset[1]);

    while (true) {
        // receive ADC data buffer
        const auto msg = sample_queue->receive_msg();
        n_consumer_samples++;
    
        constexpr auto rx_len{data_queue::DATA_BUF_LEN};
        // save data to circular buffers
        buf_a.write(msg->buffer_a, rx_len);
        buf_b.write(msg->buffer_b, rx_len);
        // write data to detectors
        det_a.write(msg->buffer_a, rx_len);
        det_b.write(msg->buffer_b, rx_len);
        data_cnt += rx_len;
        sample_queue->receive_msg_return(msg);
    
        // Tap on raw data
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

        // Save detected objects
        while (det_a.results.size() > 0) {
            auto r{det_a.results.front()};
            det_a.results.pop_front();
            r.source = 0;
            xQueueSendToBack(detector_results_q, &r, 0);
        }
        while (det_b.results.size() > 0) {
            auto r{det_b.results.front()};
            det_b.results.pop_front();
            r.source = 1;
            xQueueSendToBack(detector_results_q, &r, 0);
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

    detector_results_q = xQueueCreate(2048, sizeof(detector::detected_object_t));
    circ_buf_tap = std::make_shared<data_queue::DataTap<circular_buf_tap_t>>();

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
