#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "cic.h"

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#ifndef PLATFORM_NATIVE
#define EXECUTE_FROM_RAM __attribute__ ((long_call, section (".time_critical"))) 
#else
#define EXECUTE_FROM_RAM
#endif

void cic_init(cic_filter_t *filter,
              uint32_t order /* M */, 
             uint32_t downsample/* R */) {  
    memset(filter, 0, sizeof(cic_filter_t));
    filter->order = order;
    filter->decimation_factor = downsample; 
    filter->data_counter = downsample;

    uint32_t n = 1;
    while (order--)
        n *= downsample;
    uint32_t actual_gain = n; 
    filter->attenuate_shift = 32 - __builtin_clz(n) - 1;
    filter->gain = (float)actual_gain / (1UL << filter->attenuate_shift);
}

EXECUTE_FROM_RAM 
void cic_write(cic_filter_t *filter, const uint16_t *data, size_t length, size_t step) {
    uint32_t n, ord;
    int32_t  stage_in;
    uint32_t data_counter = filter->data_counter;
    uint32_t order = filter->order;

    for (n = 0; likely(n < length); n += step) {
        int32_t *state = filter->int_state;

        // Do integrator operation
        stage_in = data[n];
        ord = order;
        while (likely(ord--)) {
            stage_in = *state++ = stage_in + *state;
        }

        // Do decimation
        if (unlikely(--data_counter == 0))
            data_counter = filter->decimation_factor;
        else
            continue;
            
        // Do comb
        ord = order;
        while (likely(ord--)) {
            int32_t prev_in = stage_in;
            stage_in = stage_in - *state;
            *state++ = prev_in;
        }

        if (likely(filter->out_cnt < F_CIC_OUTPUT_LEN)) {
            filter->out_buf[filter->out_cnt++] = stage_in;
        } else {
            filter->overflow_cnt++;
        }
    }

    filter->data_counter = data_counter;
}

EXECUTE_FROM_RAM
size_t cic_read(cic_filter_t *filter, uint16_t *out, size_t max_length) {
    size_t output_size = filter->out_cnt;
    if (max_length < filter->out_cnt)
        max_length = filter->out_cnt;
    if (!output_size)
        return output_size;
    for (size_t n = 0; n < output_size; n++) {
        int32_t val = filter->out_buf[n] >> filter->attenuate_shift;
        if (val < 0)
            val = 0;
        if (val > UINT16_MAX) 
            val = UINT16_MAX;
        out[n] = val;
    }
    if (output_size == filter->out_cnt)
        filter->out_cnt = 0;
    else {
        memmove(filter->out_buf, filter->out_buf + output_size, sizeof(filter->out_buf[0]) * (filter->out_cnt - output_size));
        filter->out_cnt -= output_size;
    }

    return output_size;
}
