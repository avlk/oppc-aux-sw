#ifndef _SIGNAL_CHAIN_H
#define _SIGNAL_CHAIN_H

#include "analog.h"

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

typedef struct {
    uint32_t dma_samples;
    uint32_t filter_in[2];
    uint32_t filter_out[2];
    uint32_t consumer_samples;
    uint32_t correlator_runs;
    uint32_t correlator_runtime;
} signal_chain_stat_t;

const signal_chain_stat_t *get_signal_chain_stat();
std::shared_ptr<data_queue::DataTap<circular_buf_tap_t>> get_circ_buf_tap();
QueueHandle_t get_detector_results_q();

void analog_task(void *pvParameters);
void correlator_task(void *pvParameters);

void init_signal_chain();

#endif
