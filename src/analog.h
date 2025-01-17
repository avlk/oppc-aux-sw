#pragma once
#include "stdint.h"

std::pair<float,float> measure_avg_voltage(int channel, uint32_t duration_ms);


class DefaultADCConsumer: public IADCDataConsumer {
public:
    DefaultADCConsumer() : IADCDataConsumer() { }
    virtual ~DefaultADCConsumer() = default;
    void adc_consume(const uint16_t *buf, size_t buf_len) override {  }
    void adc_event(adc_event_t event) override { }
};
