#include "board_def.h"
#include <string.h>
#include <algorithm>
#include "adc.h"
#include "analog.h"
#include "filter.h"
#include "correlator.h"
#include "detector.h"
#include "signal_chain.h"


static signal_chain_stat_t stat{0};


QueueHandle_t detector_results_q{nullptr};
QueueHandle_t detector_int_q{nullptr};
QueueHandle_t correlator_results_q{nullptr};
static std::shared_ptr<data_queue::DataTap<circular_buf_tap_t>> circ_buf_tap{nullptr};
static std::shared_ptr<data_queue::DataTap<correlator_tap_t>> correlator_tap{nullptr};

const signal_chain_stat_t *get_signal_chain_stat() {
    return &stat;
}

std::shared_ptr<data_queue::DataTap<circular_buf_tap_t>> get_circ_buf_tap() {
    return circ_buf_tap;
}

QueueHandle_t get_detector_results_q() {
    return detector_results_q;
}

QueueHandle_t get_correlator_results_q() {
    return correlator_results_q;
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

    constexpr unsigned int len_threshold{10};
    constexpr unsigned int det_threshold{8};
    detector::ObjectDetector det_a(det_threshold, len_threshold);
    detector::ObjectDetector det_b(det_threshold, len_threshold);
    filter::DCBlockFilter filter_dc_a;
    filter::DCBlockFilter filter_dc_b;

    // dc_a.preinit(adc_channel_offset[0]);
    // dc_b.preinit(adc_channel_offset[1]);

    adc_set_default_dma(&default_dma_config);

    float cic_gain{filter_first_a.gain()};

    while (true) {
        const queued_adc::adc_queue_msg_t *msg;
        constexpr size_t out_buf_len{ADC_BUF_LEN/2};
        size_t len;
        int16_t out_buf[out_buf_len];

        // receive ADC data buffer
        stat.dma_samples++;
        msg = consumer->receive_msg(10);
        
        if (!msg) // should not happen
            continue;

        // .. process incoming stage
        stat.filter_in += ADC_BUF_LEN/2;
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
        // output lengths of _a and _b filters are equal
        // DC block filter output = input
        size_t first_fill = filter_first_a.out_len();
        if (first_fill > 16) {
            filter_second_a.write(filter_first_a.out_buf(), first_fill);
            filter_first_a.consume(first_fill);

            filter_second_b.write(filter_first_b.out_buf(), first_fill);
            filter_first_b.consume(first_fill);
        }

        // process detectors and data sink
        size_t second_fill{std::min(filter_second_a.out_len(), filter_second_b.out_len())};
        if (second_fill >= 16) {
            size_t max_read{data_queue::DATA_BUF_LEN - data_sink_fill};
            size_t to_read{std::min(second_fill, max_read)};

            // remove dc
            filter_dc_a.write(filter_second_a.out_buf(), to_read);
            filter_dc_b.write(filter_second_b.out_buf(), to_read);
            filter_second_a.consume(to_read);
            filter_second_b.consume(to_read);

            // fill detectors
            det_a.write(filter_dc_a.out_buf(), to_read);
            det_b.write(filter_dc_b.out_buf(), to_read);

            // fill data sink
            if (data_sink != nullptr) {
                memcpy(data_sink->buffer_a + data_sink_fill, filter_dc_a.out_buf(), to_read*sizeof(data_sink->buffer_a[0]));
                memcpy(data_sink->buffer_b + data_sink_fill, filter_dc_b.out_buf(), to_read*sizeof(data_sink->buffer_b[0]));
                data_sink_fill += to_read;
    
                if (data_sink_fill == data_queue::DATA_BUF_LEN) {
                        // Send message
                    sample_queue->send_msg_send(data_sink);
                    // Get another message or nullptr
                    data_sink = sample_queue->send_msg_claim();
                    data_sink_fill = 0;
                }
            }

            // free filter data
            filter_dc_a.consume(to_read);
            filter_dc_b.consume(to_read);
            stat.filter_out += to_read;
        }

        // Save detected objects
        while (det_a.results.size() > 0) {
            auto r{det_a.results.front()};
            det_a.results.pop_front();
            r.source = 0;
            xQueueSendToBack(detector_results_q, &r, 0);
            xQueueSendToBack(detector_int_q, &r, 1);
            stat.detector_out++;
        }
        while (det_b.results.size() > 0) {
            auto r{det_b.results.front()};
            det_b.results.pop_front();
            r.source = 1;
            xQueueSendToBack(detector_results_q, &r, 0);
            xQueueSendToBack(detector_int_q, &r, 1);
            stat.detector_out++;
        }
    }
}

void correlator_task(void *pvParameters) {
    sample_queue = std::make_shared<data_queue::QueuedDataConsumer>();
    constexpr unsigned int fs{16000};
    constexpr unsigned int ms{fs/1000};
    constexpr unsigned int processing_interval{1000*ms};
    constexpr unsigned int offset_a_min{25*ms};
    constexpr unsigned int offset_a_max{300*ms};

    // Circular buffers are for debug only
    correlator::CircularBuffer buf_a(600*ms);
    correlator::CircularBuffer buf_b(150*ms);
    size_t data_cnt{0};
    
    while (true) {
        // receive ADC data buffer
        const auto msg = sample_queue->receive_msg();
        stat.correlator_in++;
    
        constexpr auto rx_len{data_queue::DATA_BUF_LEN};
        // save data to circular buffers
        buf_a.write(msg->buffer_a, rx_len);
        buf_b.write(msg->buffer_b, rx_len);
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

        // Run correlator
        if (data_cnt >= processing_interval) {
            data_cnt = 0;

            stat.correlator_runs++;

            TickType_t start = xTaskGetTickCount();

            int32_t max_index{0};
            uint64_t max_val{0};
            constexpr size_t bin_step{5*ms};
            constexpr size_t num_bins{(offset_a_max - offset_a_min)/bin_step};
            static uint64_t bins[num_bins];
            memset(bins, 0, sizeof(bins));

            correlator::correlate_callback_t f([&](int32_t offset, uint64_t sum) {
                const auto bin_no = (offset - offset_a_min)/bin_step;
                bins[bin_no] += sum;
                if (sum > max_val) {
                    max_val = sum;
                    max_index = bin_no;
                }
            }); 

            correlator::correlate(buf_a, buf_b, offset_a_min, offset_a_max, buf_b.get_capacity(), f);

            TickType_t stop = xTaskGetTickCount();
            stat.correlator_runtime = (stop - start) * portTICK_PERIOD_MS; // ms

            correlator_result_t res;
            res.source = 0;
            res.offset = (offset_a_min + max_index * bin_step) / ms;
            res.peak = max_val;
            xQueueSendToBack(correlator_results_q, &res, 0);

            if (correlator_tap->is_triggered()) {
                correlator_tap_t tap;
                for (size_t m = 0; m < num_bins; m++) {
                    tap.index = (offset_a_min + max_index * bin_step) / ms;
                    tap.peak = (float)bins[m];
                    correlator_tap->send(tap);
                }
                correlator_tap->complete();
            }
        }       
    }
}

static bool detected_obj_compare_time(const detector::detected_object_t &a, 
                                     const detector::detected_object_t &b) {
    return a.start < b.start;
}

void detector_task(void *pvParameters) {
    std::deque<detector::detected_object_t> obj_a;
    std::deque<detector::detected_object_t> obj_b;
    constexpr size_t min_fill{100};
    constexpr size_t history_size{250};
    constexpr unsigned int fs{16000};
    constexpr unsigned int ms{fs/1000};
    constexpr int calculation_interval{5};

    int n_input{0};

    while (true) {
        detector::detected_object_t rx_obj;

        if (xQueueReceive(detector_int_q, &rx_obj, 100) != pdPASS)
            continue;

        // store items
        if (rx_obj.source == 0)
            obj_a.push_back(rx_obj);
        else
            obj_b.push_back(rx_obj);

        // remove outdated items
        while (obj_a.size() > history_size) 
            obj_a.pop_front();
        while (obj_b.size() > history_size) 
            obj_b.pop_front();
        
        if ((obj_a.size() >= min_fill) && (obj_b.size() >= min_fill)) {

            if (++n_input < calculation_interval)
                continue;
            else
                n_input = 0;

            // do object correlation
            constexpr size_t max_delay{500*ms};
            constexpr size_t bin_step{2*ms};
            constexpr size_t num_bins{max_delay/bin_step};
            std::array<uint32_t, num_bins> bins{};

            for (const auto &a: obj_a) {
                auto b_left = std::lower_bound(obj_b.cbegin(), obj_b.cend(), a, detected_obj_compare_time);
            
                for (auto b = b_left; b != obj_b.cend(); b++) {
                    if (a.start > b->start)
                        continue;
                    uint64_t delay = b->start - a.start;
                    if (delay > max_delay)
                        break; // No reason to look further
                    auto bin = delay / bin_step;
                    if (bin < num_bins)
                        bins[bin]++;
                }
            }

            const auto max_it = std::max_element(bins.cbegin(), bins.cend());
            const auto max_index = max_it - bins.cbegin();
            const auto max_offset = (max_index * bin_step) / ms;

            correlator_result_t res;
            res.source = 1;
            res.offset = max_offset;
            res.peak = *max_it;
            xQueueSendToBack(correlator_results_q, &res, 0);
        }
    }
}

void init_signal_chain() {
    detector_results_q = xQueueCreate(2048, sizeof(detector::detected_object_t));
    detector_int_q = xQueueCreate(32, sizeof(detector::detected_object_t));
    circ_buf_tap = std::make_shared<data_queue::DataTap<circular_buf_tap_t>>();

    correlator_results_q = xQueueCreate(128, sizeof(correlator_result_t));
    correlator_tap = std::make_shared<data_queue::DataTap<correlator_tap_t>>();
}

