#pragma once
#include "stdint.h"
#include "adc.h"
#include <FreeRTOS.h>
#include <queue.h>

std::pair<float,float> measure_avg_voltage(int channel, uint32_t duration_ms);

class DefaultADCConsumer : public IADCDataConsumer {
public:
    DefaultADCConsumer() : IADCDataConsumer() { }
    virtual ~DefaultADCConsumer() = default;
    virtual bool adc_consume(const uint16_t *buf, size_t buf_len) override { return false; }
    virtual void adc_event(adc_event_t event) override { }
};


namespace queued_adc {
typedef struct {
    uint32_t timestamp;
    uint16_t buffer[ADC_BUF_LEN];
} adc_queue_msg_t;

constexpr size_t ADC_QUEUE_LEN{8};
class QueuedADCConsumer : public IADCDataConsumer {
public:
    QueuedADCConsumer(); 
    virtual ~QueuedADCConsumer() = default;
    virtual bool adc_consume(const uint16_t *buf, size_t buf_len) override;
    virtual void adc_event(adc_event_t event) override;

    const adc_queue_msg_t *receive_msg(uint32_t timeout_ms) { 
        return receive_msg_int(timeout_ms / portTICK_PERIOD_MS);
    }
    const adc_queue_msg_t *receive_msg() { 
        return receive_msg_int();
    }
    void return_msg(const adc_queue_msg_t *msg);

    uint32_t cnt_no_buf{0};
    uint32_t cnt_send_fail{0};
    uint32_t cnt_return_fail{0};
private:
    const adc_queue_msg_t *receive_msg_int(TickType_t timeout = portMAX_DELAY);
    bool m_skip_first{true};
    adc_queue_msg_t m_pool[ADC_QUEUE_LEN];
    QueueHandle_t m_msg_queue;
    QueueHandle_t m_return_queue;
};

}
