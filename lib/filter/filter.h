#pragma once
#include <stdint.h>
#include <vector>
#include <deque>

namespace filter {

constexpr size_t FILTER_OUTPUT_LEN{32};
constexpr size_t MAX_FILTER_ORDER{128};
constexpr size_t FILTER_BUFFER_SIZE{MAX_FILTER_ORDER};

class FIRFilter final {
public:
    FIRFilter(std::vector<float> coefficients, size_t decimation_factor) 
        : m_decimation_factor{decimation_factor}, m_coefficients{coefficients} { }
    ~FIRFilter() = default;

    void write(uint16_t *data, size_t length, size_t step = 1);
    size_t out_len() { return m_values.size(); }
    size_t read(float *out, size_t max_length);
private:
    std::vector<float> m_coefficients;
    std::deque<float> m_values;
    size_t      m_decimation_factor{1};
    uint16_t    m_buffer[FILTER_BUFFER_SIZE];
    size_t      m_buffer_pos{0};
    size_t      m_buffer_fill{0};
    size_t      m_input_cnt{0};
    size_t      m_max_values{FILTER_OUTPUT_LEN};

    float process_one();
};

}
