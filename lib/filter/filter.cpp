#include <math.h>
#include "filter.h"


using namespace filter;

void FIRFilter::write(const uint16_t *data, size_t length, size_t step) {
    auto coefficients_size = m_coefficients.size();

    for (size_t in_ptr = 0; in_ptr < length; in_ptr += step) {
        // process input data
        m_buffer[m_buffer_pos++] = data[in_ptr];
        if (m_buffer_pos == FILTER_BUFFER_SIZE)
            m_buffer_pos = 0;

        if (m_buffer_fill < FILTER_BUFFER_SIZE)
            m_buffer_fill++;

        // Output data each m_decimation_factor'th input cycle
        bool sample_output = m_input_cnt == 0;
        // Count number of input data, wrap over decimation factor
        m_input_cnt++;
        if (m_input_cnt == m_decimation_factor)
            m_input_cnt = 0;

        if (!sample_output)
            continue;

        // Only proceed when there is enough data
        if (m_buffer_fill < coefficients_size)
            continue;

        // Do not calculate filter output when output buffer is full
        if (m_values.size() < m_max_values) {
            // put value into the buffer
            m_values.push_back(process_one());
            samples_out_cnt++;
        } else {
            overflow_cnt++;
        }
    }
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
    int p_data2 = m_buffer_pos - coeff_len;
    for (size_t n = 0; n < coeff_len/2; n++) {
        // Wrap pointers over the buffer
        if (p_data1 < 0)
            p_data1 += FILTER_BUFFER_SIZE;
        if (p_data2 < 0)
            p_data2 += FILTER_BUFFER_SIZE;
        result += ((int32_t)m_buffer[p_data1] + (int32_t)m_buffer[p_data2])  * coeff[n];
        p_data1--;
        p_data2++;
    }
    // Wrap pointer last time and use it as last value pointer
    if (p_data1 < 0)
        p_data1 += FILTER_BUFFER_SIZE;
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

void CICFilter::write(const uint16_t *data, size_t length, size_t step) {
    size_t n, ord;

    for (n = 0; n < length; n += step) {
        // Do integrator operation
        int32_t stage_in = data[n];
        for (ord = 0; ord < m_order; ord++) {
            int32_t stage_out = stage_in + m_int_delay[ord];
            m_int_delay[ord] = stage_out;
            stage_in = stage_out; 
        }
        // Do decimation
        m_data_counter++;
        if (m_data_counter < m_decimation_factor)
            continue;
        else
            m_data_counter = 0;
        // Do comb
        for (ord = 0; ord < m_order; ord++) {
            int32_t stage_out = stage_in - m_comb_delay[ord];
            m_comb_delay[ord] = stage_in;
            stage_in = stage_out;
        }

        // Do not calculate filter output when output buffer is full
        if (m_values.size() < m_max_values) {
            // put value into the buffer
            // TODO: here we have to downscale it (1/m_decimation_factor)
            m_values.push_back(stage_in);
            samples_out_cnt++;
        } else {
            overflow_cnt++;
        }

    }
}

// Returns requested number of samples or less
size_t CICFilter::read(uint16_t *out, size_t max_length) {
    size_t output_size = std::min(m_values.size(), max_length);
    if (!output_size)
        return output_size;
    // saturated copy to output
    for (size_t n = 0; n < output_size; n++) {
        auto val = m_values[n] >> m_attenuate_shift;
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
