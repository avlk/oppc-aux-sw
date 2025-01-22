#pragma once
#include "stdint.h"
#include "adc.h"

std::pair<float,float> measure_avg_voltage(int channel, uint32_t duration_ms);


class DefaultADCConsumer : public IADCDataConsumer {
public:
    DefaultADCConsumer() : IADCDataConsumer() { }
    virtual ~DefaultADCConsumer() = default;
    virtual void adc_consume(const uint16_t *buf, size_t buf_len) override {  }
    virtual void adc_event(adc_event_t event) override { }
};

class DualChannelADCConsumer : public IADCDataConsumer {
public:
    DualChannelADCConsumer(std::shared_ptr<TimeSeriesDataConsumer> consumer_a, 
                           std::shared_ptr<TimeSeriesDataConsumer> consumer_b) 
                        : IADCDataConsumer() {
        m_consumers[0] = consumer_a;
        m_consumers[1] = consumer_b;
    }
    virtual ~DualChannelADCConsumer() = default;
    virtual void adc_consume(const uint16_t *buf, size_t buf_len) override;
    virtual void adc_event(adc_event_t event) override;
private:
    bool m_skip_first{true};
    int buffer_a[ADC_BUF_LEN/2];
    int buffer_b[ADC_BUF_LEN/2];
    std::shared_ptr<TimeSeriesDataConsumer> m_consumers[2];
};
