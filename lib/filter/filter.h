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
    FIRFilter(std::vector<float> coefficients, size_t decimation_factor,
              int32_t gain = 4096) 
        : m_decimation_factor{decimation_factor}, m_gain(gain) {
        set_coefficients(coefficients, gain);
    }
    ~FIRFilter() = default;

    void write(uint16_t *data, size_t length, size_t step = 1);
    size_t out_len() { return m_values.size(); }
    size_t read(int32_t *out, size_t max_length);
    size_t read(uint16_t *out, size_t max_length);

    bool is_symmetric() { return m_is_symmetric; }
    uint32_t overflow_cnt{0};
    uint32_t samples_out_cnt{0};
private:
    std::vector<int32_t> m_coefficients;
    std::deque<int32_t> m_values;
    size_t      m_decimation_factor{1};
    uint16_t    m_buffer[FILTER_BUFFER_SIZE];
    size_t      m_buffer_pos{0};
    size_t      m_buffer_fill{0};
    size_t      m_input_cnt{0};
    size_t      m_max_values{FILTER_OUTPUT_LEN};
    bool        m_is_symmetric{false};
    int32_t    m_gain;

    void set_coefficients(std::vector<float> coefficients, int32_t gain);
    int32_t process_one();
    int32_t process_one_sym();
};

}
