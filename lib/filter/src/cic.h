#ifndef CIC_FILTER_C_H
#define CIC_FILTER_C_H

#include <stdint.h>
#include <stdlib.h>

#define F_CIC_MAX_ORDER 8
#define F_CIC_OUTPUT_LEN 128

typedef struct {
    uint8_t    order;
    uint8_t    decimation_factor;
    uint8_t    data_counter;
    uint8_t    attenuate_shift;
    uint32_t    out_cnt;
    int32_t     int_state[F_CIC_MAX_ORDER*2];
    int32_t     out_buf[F_CIC_OUTPUT_LEN];
    uint32_t    overflow_cnt;
    uint32_t    samples_out_cnt;
    float       gain;
} cic_filter_t;

void cic_init(cic_filter_t *filter, uint32_t order /* M */, uint32_t downsample/* R */);
void cic_write(cic_filter_t *filter, const uint16_t *data, size_t length, size_t step);
size_t cic_read(cic_filter_t *filter, uint16_t *out, size_t max_length);


#endif
