#include "filter.h"


using namespace filter;

void FIRFilter::write(uint16_t *data, size_t length, size_t step) {

    size_t in_ptr;
    for (in_ptr = 0; in_ptr < length; in_ptr += step) {
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
        // Do not calculate filter output when output buffer is full
        if ((m_buffer_fill >= m_coefficients.size()) && 
            (m_values.size() < m_max_values)) {
            // put value into the buffer
            m_values.push_back(process_one());
        }
    }
}


// Calculates one sample of filter output for the last input value
float FIRFilter::process_one() {
    auto coeff_len = m_coefficients.size();
    auto coeff = m_coefficients.data();

    float result = 0;
    for (size_t n = 0; n < coeff_len; n++) {
        int p_data = m_buffer_pos - 1 - n;
        if (p_data < 0)
            p_data += FILTER_BUFFER_SIZE;
        result += (float)m_buffer[p_data] * coeff[n];
    }
    return result;
}

// Returns requested number of samples or less
size_t FIRFilter::read(float *out, size_t max_length) {
    size_t output_size = std::max(m_values.size(), max_length);
    if (!output_size)
        return output_size;
    // copy to output
    for (size_t n = 0; n < output_size; n++)
        out[n] = m_values[n];
    // free data
    m_values.erase(m_values.begin(), m_values.begin() + output_size);

    return output_size;
}
