#include <math.h>
#include <string.h>
#include "filter.h"


using namespace filter;


#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#ifndef PLATFORM_NATIVE
#define EXECUTE_FROM_RAM(subsection) __attribute__ ((long_call, section (".time_critical." subsection))) 
#else
#define EXECUTE_FROM_RAM(subsection)
#endif

EXECUTE_FROM_RAM("fir")
void FIRFilter::write(const uint16_t *data, size_t length, size_t step) {
    auto coefficients_size = m_coefficients.size();
    auto data_counter = m_data_counter;

    for (size_t in_ptr = 0; in_ptr < length; in_ptr += step) {
        // process input data
        m_buffer[m_buffer_pos++] = data[in_ptr];
        if (m_buffer_pos == FILTER_BUFFER_SIZE)
            m_buffer_pos = 0;

        // Output data each m_decimation_factor'th input cycle
        // data counter is 
        if (likely(--data_counter == 0))
            data_counter = m_decimation_factor;
        else
            continue;

        // Do not calculate filter output when output buffer is full
        if (m_values.size() < m_max_values) {
            // put value into the buffer
            m_values.push_back(process_one());
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
    if (m_is_symmetric)
        return process_one_sym();
    // Generic filter implementation
    auto coeff_len = m_coefficients.size();
    auto coeff = m_coefficients.data();

    int32_t result = 0;
    int p_data = m_buffer_pos - 1;
    for (size_t n = 0; n < coeff_len; n++) {
        if (p_data < 0)
            p_data += FILTER_BUFFER_SIZE;
        result += (int32_t)m_buffer[p_data] * coeff[n];
        p_data--;
    }
    return result >> m_gain_bits;
}

EXECUTE_FROM_RAM("fir")
int32_t FIRFilter::process_one_sym() {
    auto coeff_len = m_coefficients.size();
    auto coeff = m_coefficients.data();

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
    int p_data1 = m_buffer_pos - 1;
    if (p_data1 < 0)
        p_data1 += FILTER_BUFFER_SIZE;

    int p_data2 = m_buffer_pos - coeff_len;
    if (p_data2 < 0)
        p_data2 += FILTER_BUFFER_SIZE;

    for (size_t n = 0; n < coeff_len/2; n++) {
        // Wrap pointers over the buffer
        result += ((int32_t)m_buffer[p_data1] + (int32_t)m_buffer[p_data2])  * coeff[n];
        if (--p_data1 < 0)
            p_data1 += FILTER_BUFFER_SIZE;
        if (++p_data2 == FILTER_BUFFER_SIZE)
            p_data2 = 0;
    }
    // Wrap pointer last time and use it as last value pointer
    result += (int32_t)m_buffer[p_data1] * coeff[coeff_len/2];

    return result >> m_gain_bits;
}

// Returns requested number of samples or less
size_t FIRFilter::read(int32_t *out, size_t max_length) {
    size_t output_size = std::min(m_values.size(), max_length);
    if (!output_size)
        return output_size;
    // copy to output
    for (size_t n = 0; n < output_size; n++)
        out[n] = m_values[n];
    // free data
    m_values.erase(m_values.begin(), m_values.begin() + output_size);

    return output_size;
}

// Returns requested number of samples or less
size_t FIRFilter::read(uint16_t *out, size_t max_length) {
    size_t output_size = std::min(m_values.size(), max_length);
    if (!output_size)
        return output_size;
    // saturated copy to output
    for (size_t n = 0; n < output_size; n++) {
        auto val = m_values[n];
        if (val < 0)
            val = 0;
        if (val > UINT16_MAX) 
            val = UINT16_MAX;
        out[n] = val;
    }
    // free data
    m_values.erase(m_values.begin(), m_values.begin() + output_size);

    return output_size;
}

template <uint8_t order /* M */, uint8_t decimation_factor /* R */>
EXECUTE_FROM_RAM("cic")
void CICFilter<order, decimation_factor>::write(const uint16_t *data, size_t length, size_t step) {
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
            m_out_buf[m_out_cnt++] = stage_in;
        } else {
            overflow_cnt++;
        }
    }

    m_data_counter = data_counter;
}

// Returns requested number of samples or less
template <uint8_t order /* M */, uint8_t decimation_factor /* R */>
EXECUTE_FROM_RAM("cic")
size_t CICFilter<order, decimation_factor>::read(uint16_t *out, size_t max_length) {
    size_t output_size = std::min(m_out_cnt, max_length);
    if (!output_size)
        return output_size;
    for (size_t n = 0; n < output_size; n++) {
        auto val = m_out_buf[n] >> m_attenuate_shift;
        if (val < 0)
            val = 0;
        if (val > UINT16_MAX) 
            val = UINT16_MAX;
        out[n] = val;
    }
    if (output_size == m_out_cnt)
        m_out_cnt = 0;
    else {
        memmove(m_out_buf, &m_out_buf[output_size], sizeof(m_out_buf[0]) * (m_out_cnt - output_size));
        m_out_cnt = m_out_cnt - output_size;
    }

    return output_size;
}


// Pre-instantiate templated classes for requested cases
template class CICFilter<4,5>;