#include <math.h>
#include <string.h>
#include <FreeRTOS.h>
#include <queue.h>
#include "message_buffer.h"
#include "filter.h"


using namespace filter;


#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#ifndef PLATFORM_NATIVE
#define EXECUTE_FROM_RAM(subsection) __attribute__ ((long_call, section (".time_critical." subsection))) 
#else
#define EXECUTE_FROM_RAM(subsection)
#endif

static MessageBufferHandle_t tap_stream;
void filter::filter_init() {
    constexpr size_t storage_length{4};
    tap_stream = xMessageBufferCreate((sizeof(size_t) + sizeof(filter_tap_t))*storage_length);
}

static void filter_tap_send(int id, const int16_t *data, size_t len, bool done);


EXECUTE_FROM_RAM("filter")
void GenericFilter::consume(size_t max_length) {
    size_t consume_size = std::min(m_out_cnt, max_length);
    if (!consume_size)
        return;

    if (unlikely(m_tap_active)) {
        bool final{false};
        if (consume_size >= m_tap_len) {
            m_tap_len = 0;
            m_tap_active = 0;
            final = true;
        } else {
            m_tap_len -= consume_size;
        }
        filter_tap_send(m_tap_id, m_out_buf, consume_size, final);
    }

    if (consume_size == m_out_cnt)
        m_out_cnt = 0;
    else {
        memmove(m_out_buf, &m_out_buf[consume_size], sizeof(m_out_buf[0]) * (m_out_cnt - consume_size));
        m_out_cnt = m_out_cnt - consume_size;
    }
}

// Returns requested number of samples or less
EXECUTE_FROM_RAM("filter")
size_t GenericFilter::read(int16_t *out, size_t max_length) {
    size_t output_size = std::min(m_out_cnt, max_length);
    if (!output_size)
        return output_size;
    // copy to output
    memcpy(out, m_out_buf, output_size * sizeof(m_out_buf[0]));
    // remove from buffer
    consume(output_size);
    return output_size;
}


EXECUTE_FROM_RAM("fir")
void FIRFilter::write(const int16_t *data, size_t length, size_t step) {
    auto coefficients_size = m_coefficients.size();
    auto data_counter = m_data_counter;

    for (size_t in_ptr = 0; in_ptr < length; in_ptr += step) {
        // process input data
        m_buffer[m_buffer_pos] = data[in_ptr];
        m_buffer_pos = (m_buffer_pos + 1) & FILTER_ADDR_MASK;

        // Output data each m_decimation_factor'th input cycle
        // data counter is 
        if (likely(--data_counter == 0))
            data_counter = m_decimation_factor;
        else
            continue;

        // Do not calculate filter output when output buffer is full
        if (likely(m_out_cnt < FILTER_OUTPUT_LEN)) {
            // put value into the buffer
            int32_t out_val = process_one();
            if (out_val > INT16_MAX)
                out_val = INT16_MAX;
            if (out_val < INT16_MIN)
                out_val = INT16_MIN;
            m_out_buf[m_out_cnt++] = out_val; 
        } else {
            overflow_cnt++;
        }
    }

    m_data_counter = data_counter;
}

void FIRFilter::set_coefficients(std::vector<float> coefficients, uint32_t gain_bits) {
    size_t n;
    int32_t gain = 1 << gain_bits;

    for (n = 0; n < coefficients.size(); n++) {
        m_coefficients.push_back(round(coefficients[n] * gain));
    }

    auto coeff_len = m_coefficients.size();
    auto coeff = m_coefficients.data();
    bool sym = true;

    // Check if a filter is symmetric
    for (n = 0; n < coeff_len/2; n++) {
        if (coeff[n] != coeff[coeff_len - 1 - n]) {
            sym = false;
            break;
        }
    }

    m_is_symmetric = sym;
}


// Calculates one sample of filter output for the last input value
EXECUTE_FROM_RAM("fir")
int32_t FIRFilter::process_one() {
    if (likely(m_is_symmetric))
        return process_one_sym();
    // Generic filter implementation
    const auto coeff_len = m_coefficients.size();
    const auto coeff = m_coefficients.data();
    const auto buf = &m_buffer[0];

    int32_t result = 0;
    uint32_t p_data = (m_buffer_pos - 1) & FILTER_ADDR_MASK;
    for (size_t n = 0; n < coeff_len; n++) {
        result += (int32_t)buf[p_data] * coeff[n];
        p_data = (p_data - 1) & FILTER_ADDR_MASK;
    }
    return result >> m_gain_bits;
}

EXECUTE_FROM_RAM("fir")
int32_t FIRFilter::process_one_sym() {
    const auto coeff_len = m_coefficients.size();
    const auto coeff = m_coefficients.data();
    const auto buf = &m_buffer[0];

    // for 7-tap filter:
    // coeff_len = 7
    // data pairs for main loop: 
    // coeff[0] * (data[-1] + data[-7])
    // coeff[1] * (data[-2] + data[-6])
    // coeff[2] * (data[-3] + data[-5]) 
    // last unpaired value:
    // coeff[3] *  data[-4]

    int32_t result = 0;
    // Set initial data pointers
    uint32_t p_data1 = (m_buffer_pos - 1) & FILTER_ADDR_MASK;
    uint32_t p_data2 = (m_buffer_pos - m_coefficients.size()) & FILTER_ADDR_MASK;

    for (size_t n = 0; n < coeff_len/2; n++) {
        // Wrap pointers over the buffer
        result += ((int32_t)buf[p_data1] + (int32_t)buf[p_data2])  * coeff[n];
        p_data1 = (p_data1 - 1) & FILTER_ADDR_MASK;
        p_data2 = (p_data2 + 1) & FILTER_ADDR_MASK;
    }
    // Wrap pointer last time and use it as last value pointer
    result += (int32_t)buf[p_data1] * coeff[coeff_len/2];

    return result >> m_gain_bits;
}

template <uint8_t order /* M */, uint8_t decimation_factor /* R */>
EXECUTE_FROM_RAM("cic")
void CICFilter<order, decimation_factor>::write(const int16_t *data, size_t length, 
                                                size_t step) {
    size_t n, ord;
    int32_t stage_in;
    uint32_t data_counter = m_data_counter;

    for (n = 0; n < length; n += step) {
        int32_t *state = &m_int_state[0];

        // Do integrator operation
        stage_in = data[n];
        ord = order;
        while (likely(ord--)) {
            stage_in = *state++ = stage_in + *state;
        }

        
        // Do decimation
        if (likely(--data_counter == 0))
            data_counter = decimation_factor;
        else
            continue;

        // Do comb
        ord = order;
        while (likely(ord--)) {
            int32_t prev_in = stage_in;
            stage_in = stage_in - *state;
            *state++ = prev_in;
        }

        // Do not calculate filter output when output buffer is full
        if (likely(m_out_cnt < FILTER_OUTPUT_LEN)) {
            // downscale, clip and truncate
            int32_t out_val = stage_in / (1 << m_attenuate_shift);
            if (out_val > INT16_MAX)
                out_val = INT16_MAX;
            if (out_val < INT16_MIN)
                out_val = INT16_MIN;
            m_out_buf[m_out_cnt++] = out_val;
        } else {
            overflow_cnt++;
        }
    }

    m_data_counter = data_counter;
}


// Pre-instantiate templated classes for requested cases
template class CICFilter<4,5>;


EXECUTE_FROM_RAM("dcblock")
void DCBlockFilter::write(const int16_t *data, size_t length) {
    auto dc_acc = m_dc_acc;
    auto dc_prev_x = m_dc_prev_x;
    auto dc_prev_y = m_dc_prev_y;

    while (length--) {
        int32_t sample = *data++;

        // DC removal stage
        // https://dspguru.com/dsp/tricks/fixed-point-dc-blocking-filter-with-noise-shaping/
        // https://www.iro.umontreal.ca/~mignotte/IFT3205/Documents/TipsAndTricks/DCBlockerAlgorithms.pdf
        {
            dc_acc -= dc_prev_x;
            dc_prev_x = sample << DC_BASE_SHIFT;
            dc_acc += dc_prev_x;
            dc_acc -= DC_POLE_NUM*dc_prev_y; 
            dc_prev_y = dc_acc / (1 << DC_BASE_SHIFT); // hopefully translates to ASR
        }
        auto int_y = dc_prev_y;

        if (likely(m_out_cnt < FILTER_OUTPUT_LEN)) {
            // downscale, clip and truncate
            m_out_buf[m_out_cnt++] = int_y;
        } else {
            overflow_cnt++;
        }
    }

    m_dc_acc = dc_acc;
    m_dc_prev_x = dc_prev_x;
    m_dc_prev_y = dc_prev_y;
}


static void filter_tap_send(int id, const int16_t *buf, size_t len, bool done) {
    filter_tap_t tap{};
    tap.id = id;
    tap.len = len;
    tap.done = done ? 1 : 0;
    memcpy(tap.buf, buf, len * sizeof(*buf));
    xMessageBufferSend(tap_stream, &tap, sizeof(tap), portMAX_DELAY);
}
    
bool filter::filter_tap_receive(filter_tap_t *buf, unsigned int timeout) {
    if (timeout != portMAX_DELAY)
        timeout = pdMS_TO_TICKS(timeout);
    return sizeof(filter_tap_t) == xMessageBufferReceive(tap_stream, buf, sizeof(filter_tap_t), timeout);
}
