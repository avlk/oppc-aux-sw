#include <Arduino.h>
#include "adc.h"
#include "cli_out.h"
#include <vector>
#include "analog.h"



class AVGConsumer: public IADCDataConsumer {
public:
    AVGConsumer(size_t consume_length) : m_consume_length{consume_length} { 
        m_samples = new uint16_t[m_consume_length];
    }

    virtual ~AVGConsumer() {
        delete[] m_samples;
    }

    virtual void adc_consume(const uint16_t *buf, size_t buf_len) override {
        if (m_done) {
            return;
        }

        if (m_skip_first) {
            // First block of measurements can contain samples of previous measurements
            // it's easier to skip one block than to find out what is wrong with DMA
            m_skip_first = false;
            return;
        }

        for (size_t n = 0; n < buf_len; n++) {
            if (m_samples_fill < m_consume_length)
                m_samples[m_samples_fill++] = buf[n];
        }

        if (m_samples_fill >= m_consume_length) {
            m_done = true;
            adc_stop_ooo_dma();
        }
    }

    virtual void adc_event(adc_event_t event) override {
        if (event == ADC_EVENT_DMA_START) {
            m_samples_fill = 0;
            m_skip_first = true;
        }
    }

    bool is_done() {
        return m_done;
    }
    
    std::pair<float, float> process_data() {
        size_t n;

        if (!m_samples_fill)
            return std::make_pair(.0f, .0f);
        
        uint64_t sum{0};
        for (n = 0; n < m_samples_fill; n++) {
            sum += m_samples[n];
        }
        float m_average = ((float)sum)/m_samples_fill;

        float sum_err{0};
        for (n = 0; n < m_samples_fill; n++) {
            sum_err += (m_samples[n] - m_average)*(m_samples[n] - m_average);
        }
        float m_noise = sqrt(sum_err/m_samples_fill);

        // Convert averaged values to V
        // adc_raw_to_V will also remove offset,
        // adc_scale_to_V(m_noise) will not remove offset to preserve noive figure
        return std::make_pair(adc_raw_to_V(m_average), adc_scale_to_V(m_noise));
    }
private:

    size_t m_consume_length;
    size_t m_samples_fill{0};
    uint16_t *m_samples;
    bool m_skip_first{true};
    bool m_done{false};
};


std::pair<float,float> measure_avg_voltage(int channel, uint32_t duration_ms) {
    size_t sample_rate = 250000;
    size_t data_length = (sample_rate / 1000) * duration_ms;
    if (data_length > 10240) {
        cli_debug("ADC measurement data length truncated");
        data_length = 10240;
    }

    auto consumer = std::make_shared<AVGConsumer>(data_length);

    adc_dma_config_t single_dma_config = {
        n_inputs: 1,
        inputs: {(uint8_t)channel},
        sample_freq: sample_rate,
        consumer: consumer
    };

    // Run conversion
    adc_run_ooo_dma(&single_dma_config);
    while (!consumer->is_done()) {
        sleep_ms(1);
    }
    adc_rerun_default_dma();
    return consumer->process_data();
}
