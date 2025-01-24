#include <Arduino.h>
#include <hardware/adc.h>
#include <hardware/dma.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "adc.h"
#include "board_def.h"
#include "cli_out.h"
#include "io.h"

static uint adc_dma_chan[2];
static dma_channel_config adc_dma_cfg[2];
static uint16_t adc_buf0[ADC_BUF_LEN];
static uint16_t adc_buf1[ADC_BUF_LEN];

static const adc_dma_config_t *dma_config_default = NULL;
static const adc_dma_config_t *dma_config_current = NULL;
static volatile bool dma_running = false;
int adc_offset = 16; // Typical '0' ADC offset

static void adc_calibrate();
static void setup_adc_dma(void);

void adc_begin() {
    adc_init();

    adc_gpio_init(IO_S1_AIN); 
    adc_gpio_init(IO_S2_AIN); 
    adc_gpio_init(IO_GND_AIN); 

    adc_select_input(0);

    adc_set_temp_sensor_enabled(false);

    setup_adc_dma();
    adc_calibrate();
}

void adc_calibrate() {
    task_sleep_ms(10);

    int acc = 0;
    for (int n = 0; n < 16; n++) {
        acc += adc_read_raw(ADC_CH_REF);
        task_sleep_ms(1);
    }
    adc_offset = acc / 16;
}


static void dma_irq0_handler() {
    bool higher_prio_task_woken{false};

    // Clear the interrupt request.
    dma_channel_acknowledge_irq0(adc_dma_chan[0]);
    dma_channel_set_write_addr(adc_dma_chan[0], adc_buf0, false);
    dma_channel_set_trans_count(adc_dma_chan[0], ADC_BUF_LEN, false);
    // Run consumer, it returns true is a higher priority task is woken
    if (dma_config_current && (dma_config_current->consumer != nullptr))
        higher_prio_task_woken = dma_config_current->consumer->adc_consume(adc_buf0, ADC_BUF_LEN);
    
    portYIELD_FROM_ISR(higher_prio_task_woken);
}

static void dma_irq1_handler() {
    bool higher_prio_task_woken{false};

    // Clear the interrupt request.
    dma_channel_acknowledge_irq1(adc_dma_chan[1]);
    dma_channel_set_write_addr(adc_dma_chan[1], adc_buf1, false);
    dma_channel_set_trans_count(adc_dma_chan[1], ADC_BUF_LEN, false);
    // Run consumer, it returns true is a higher priority task is woken
    if (dma_config_current && (dma_config_current->consumer != nullptr))
        higher_prio_task_woken = dma_config_current->consumer->adc_consume(adc_buf1, ADC_BUF_LEN);

    portYIELD_FROM_ISR(higher_prio_task_woken);
}

static void setup_adc_dma(void) {

    adc_run(false);

    // Setup two DMA channels
    for (int i = 0; i < 2; i++) {
        adc_dma_chan[i] = dma_claim_unused_channel(true);
        adc_dma_cfg[i] = dma_channel_get_default_config(adc_dma_chan[i]);

        channel_config_set_transfer_data_size(&adc_dma_cfg[i], DMA_SIZE_16);
        channel_config_set_read_increment(&adc_dma_cfg[i], false);
        channel_config_set_write_increment(&adc_dma_cfg[i], true);
        // Pace transfers based on availability of ADC samples
        channel_config_set_dreq(&adc_dma_cfg[i], DREQ_ADC);
    }
    // Chain DMA channels in the loop
    channel_config_set_chain_to(&adc_dma_cfg[0], adc_dma_chan[1]);
    channel_config_set_chain_to(&adc_dma_cfg[1], adc_dma_chan[0]);

    // Configure channels but don't start them yet
    dma_channel_configure(adc_dma_chan[0], &adc_dma_cfg[0], adc_buf0, &adc_hw->fifo, ADC_BUF_LEN, false);
    dma_channel_configure(adc_dma_chan[1], &adc_dma_cfg[1], adc_buf1, &adc_hw->fifo, ADC_BUF_LEN, false);

    // Tell the DMA to raise IRQ line 0/1 when the channel finishes a block
    dma_channel_set_irq0_enabled(adc_dma_chan[0], true);
    dma_channel_set_irq1_enabled(adc_dma_chan[1], true);

    // Configure the processor to run dma_irq0/1_handler() when DMA IRQ 0/1 is asserted
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq0_handler);
    irq_set_exclusive_handler(DMA_IRQ_1, dma_irq1_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    irq_set_enabled(DMA_IRQ_1, true);
}


static void adc_dma_start_int(const adc_dma_config_t *config) {
    uint32_t div = (48000000 / config->sample_freq) - 1;

    adc_set_clkdiv(div);
    if (config->n_inputs == 0) 
        return;
    if (config->n_inputs == 1) {
        adc_set_round_robin(0);
        adc_select_input(config->inputs[0]);
    } else {
        uint8_t mask = 0;
        for (uint8_t n = 0; n < config->n_inputs; n++)
            mask |= (1 << config->inputs[n]);
        adc_set_round_robin(mask);
        adc_select_input(config->inputs[0]);
    }
    
    dma_config_current = config;
    if (dma_config_current && (dma_config_current->consumer != nullptr))
        dma_config_current->consumer->adc_event(ADC_EVENT_DMA_START);
    adc_fifo_setup(true, true, 1, false, false);

    adc_run(false);
    adc_fifo_drain();
    // Start DMA channel 0 and ADC
    dma_channel_start(adc_dma_chan[0]);
    adc_run(true);
    dma_running = true;
}

static void adc_dma_stop_int(void) {
    const adc_dma_config_t *config = dma_config_current;
    dma_config_current = NULL; // Disable consumer before calling abort, because it can create spurious interrupts (RP2040 Errata)
    dma_channel_abort(adc_dma_chan[0]);
    dma_channel_abort(adc_dma_chan[1]);
    if (config && (config->consumer != nullptr))
        config->consumer->adc_event(ADC_EVENT_DMA_STOP);
    dma_running = false;
}

/* Sets default DMA which runs when no other consumers are active */
void adc_set_default_dma(const adc_dma_config_t *config) {
    dma_config_default = config;
    if (!dma_running && dma_config_default)
        adc_dma_start_int(dma_config_default);
}

/* Runs out-of-order DMA (non-default) */
void adc_run_ooo_dma(const adc_dma_config_t *config) {
    if (dma_running)
        adc_dma_stop_int();
    adc_dma_start_int(config);
}

/* Stops out-of-order DMA  */
void adc_stop_ooo_dma() {
    if (dma_running)
        adc_dma_stop_int();
}

void adc_rerun_default_dma() {
    if (dma_running)
        return;
    if (dma_config_default)
        adc_dma_start_int(dma_config_default);
}

void adc_disable_dma() {
    if (dma_running)
        adc_dma_stop_int();
}

void adc_enable_dma() {
    if (dma_running)
        return;
    if (dma_config_current)
        adc_dma_start_int(dma_config_current);
    else if (dma_config_default)
        adc_dma_start_int(dma_config_default);
}


uint32_t adc_read_raw(int input) {
    if (dma_running)
        return 0; // Should not be called if DMA is running
    adc_select_input(input);
    adc_fifo_setup(false, true, 1, false, false);
    return adc_read();
}

float adc_read_V(int input) {
    return adc_raw_to_V(adc_read_raw(input));
}

float adc_raw_to_V(int input) {
    int value = input - adc_offset;
    if (value < 0)
        value = 0;
    return adc_scale_to_V(value);
}

float adc_scale_to_V(int input) {
    return ((float)input * 3.3) / (1U << ADC_BITS);
}
