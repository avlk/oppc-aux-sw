#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <memory>

#define ADC_BITS 12
#define ADC_BUF_LEN 128

typedef enum {
    ADC_EVENT_DMA_START,
    ADC_EVENT_DMA_STOP
} adc_event_t;

class IADCDataConsumer {
public:
    // adc_consume receives (consumes) data during the ISR
    // returns true if a higher-priority task is woken by processing
    // and FreeRTOS needs to reschedule
    virtual bool adc_consume(const uint16_t *buf, size_t buf_len) = 0;
    virtual void adc_event(adc_event_t event) = 0;
};

class TimeSeriesDataConsumer {
public:
    virtual void consume(const int *buf, size_t buf_len) = 0;
    virtual void consume(const float *buf, size_t buf_len) = 0;
};



typedef struct {
    uint8_t             n_inputs;
    uint8_t             inputs[4];
    uint32_t            sample_freq;
    std::shared_ptr<IADCDataConsumer> consumer;
} adc_dma_config_t;

void adc_begin();

/* Enables and disables underground DMA conversions, including default and OOO DMA.
 * If DMA was active before adc_disable_dma, it will be restarted after adc_enable_dma.
 * Calling dut_read_adc_V() and dut_read_adc_raw() when DMA is active is not allowed. 
 */
void adc_enable_dma();
void adc_disable_dma();
/* Sets default DMA which runs when no other consumers are active */
void adc_set_default_dma(const adc_dma_config_t *config);
/* Runs out-of-order DMA (non-default). This DMA will be run until stopped
 * with adc_stop_ooo_dma(). When OOO DMA is stopped, default DMA will be re-run.
 */
void adc_run_ooo_dma(const adc_dma_config_t *config);
void adc_stop_ooo_dma();
void adc_rerun_default_dma();


/* Reads ADC input in non-DMA mode. Calling when DMA is enabled will return 0 */
uint32_t adc_read_raw(int input);   // Read ADC input is digital form
float adc_read_V(int input);        // Read ADC input in Volts
/* Converts raw ADC data to input V, taking adc offset into account */
float adc_raw_to_V(int input);
/* Converts raw ADC data to input V, NOT taking adc offset into account,
   for relative measurements */
float adc_scale_to_V(int input);
