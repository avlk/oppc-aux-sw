#include <Arduino.h>
#include "adc.h"
#include "cli_out.h"
#include "io.h"
#include "analog.h"



class AVGConsumer: public IADCDataConsumer {
public:
    AVGConsumer(size_t consume_length) : m_consume_length{consume_length} { 
        m_samples = new uint16_t[m_consume_length];
    }

    virtual ~AVGConsumer() {
        delete[] m_samples;
    }

    virtual bool adc_consume(const uint16_t *buf, size_t buf_len) override {
        if (m_done) {
            return false;
        }

        if (m_skip_first) {
            // First block of measurements can contain samples of previous measurements
            // it's easier to skip one block than to find out what is wrong with DMA
            m_skip_first = false;
            return false;
        }

        for (size_t n = 0; n < buf_len; n++) {
            if (m_samples_fill < m_consume_length)
                m_samples[m_samples_fill++] = buf[n];
        }

        if (m_samples_fill >= m_consume_length) {
            m_done = true;
            adc_stop_ooo_dma();
        }
        return false;
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
        task_sleep_ms(1);
    }
    adc_rerun_default_dma();
    return consumer->process_data();
}

#if !INCLUDE_vTaskSuspend
#error This code relies on infinite blocking of tasks when portMAX_DELAY is set as timeout value
#endif
namespace queued_adc {

QueuedADCConsumer::QueuedADCConsumer() : IADCDataConsumer() {
    // This is a queue for received data
    m_msg_queue = xQueueCreate(ADC_QUEUE_LEN, sizeof(adc_queue_msg_t *));
    // This queue holds unused buffers
    m_return_queue = xQueueCreate(ADC_QUEUE_LEN, sizeof(adc_queue_msg_t *));
    // Fill return queue with unused buffers
    for (size_t n = 0; n < ADC_QUEUE_LEN; n++) {
        adc_queue_msg_t *ptr = &m_pool[n];
        xQueueSendToBack(m_return_queue, &ptr, 0);
    }
}


bool QueuedADCConsumer::adc_consume(const uint16_t *buf, size_t buf_len) {
    if (m_skip_first) {
        // First block of measurements can contain samples of previous measurements
        // it's easier to skip one block than to find out what is wrong with DMA
        m_skip_first = false;
        return false;
    }

    if (buf_len != ADC_BUF_LEN)
        return false;

    adc_queue_msg_t *msg = nullptr;
    BaseType_t higher_prio_task_woken = pdFALSE;

    // Get free buffer
    if (xQueueReceiveFromISR(m_return_queue, &msg, &higher_prio_task_woken) != pdTRUE) {
        // Failed to get buffer for data
        cnt_no_buf++;
        return false;
    }

    // copy data
    memcpy(msg->buffer, buf, sizeof(msg->buffer));
    // TODO: for now timestamp field is not used
    msg->timestamp = 0;

    // Send the buffer - it shall not fail: as long as we got a buffer 
    // from m_return_queue, there should be a place in the send queue 
    if (xQueueSendToBackFromISR(m_msg_queue, &msg, &higher_prio_task_woken) != pdTRUE)
        cnt_send_fail++;

    return higher_prio_task_woken == pdTRUE;
}

void QueuedADCConsumer::adc_event(adc_event_t event) {
    if (event == ADC_EVENT_DMA_START) {
        m_skip_first = true;
    }
}

const adc_queue_msg_t *QueuedADCConsumer::receive_msg_int(TickType_t timeout) {
    adc_queue_msg_t *msg;

    if (xQueueReceive(m_msg_queue, &msg, timeout) != pdPASS)
        return nullptr;
    
    return msg;
}

void QueuedADCConsumer::return_msg(const adc_queue_msg_t *msg) {
    const adc_queue_msg_t *_msg{msg};

    if (xQueueSendToBack(m_return_queue, &_msg, portMAX_DELAY) != pdTRUE)
        cnt_return_fail++;
}

};

namespace data_queue {


QueuedDataConsumer::QueuedDataConsumer() {
    // This is a queue for received data
    m_msg_queue = xQueueCreate(DATA_QUEUE_LEN, sizeof(data_queue_msg_t *));
    // This queue holds unused buffers
    m_return_queue = xQueueCreate(DATA_QUEUE_LEN, sizeof(data_queue_msg_t *));
    // Fill return queue with unused buffers
    for (size_t n = 0; n < DATA_QUEUE_LEN; n++) {
        data_queue_msg_t *ptr = &m_pool[n];
        xQueueSendToBack(m_return_queue, &ptr, 0);
    }
}
  
const data_queue_msg_t *QueuedDataConsumer::receive_msg_int(TickType_t timeout) {
    data_queue_msg_t *msg;

    if (xQueueReceive(m_msg_queue, &msg, timeout) != pdPASS)
        return nullptr;
    
    return msg;
}

void QueuedDataConsumer::receive_msg_return(const data_queue_msg_t *msg) {
    const data_queue_msg_t *_msg{msg};

    if (xQueueSendToBack(m_return_queue, &_msg, portMAX_DELAY) != pdTRUE)
        cnt_return_fail++;
}

data_queue_msg_t* QueuedDataConsumer::send_msg_claim() {
    data_queue_msg_t *msg;

    if (xQueueReceive(m_return_queue, &msg, 0) != pdPASS)
        return nullptr;
    
    return msg;
}

void QueuedDataConsumer::send_msg_send(const data_queue_msg_t* msg) {
    const data_queue_msg_t *_msg{msg};

    if (xQueueSendToBack(m_msg_queue, &_msg, portMAX_DELAY) != pdTRUE)
        cnt_send_fail++;
}

};
