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
              uint32_t gain_bits = 12) 
        : m_decimation_factor{decimation_factor}, m_gain_bits(gain_bits) {
        set_coefficients(coefficients, gain_bits);
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
    uint32_t    m_gain_bits;

    void set_coefficients(std::vector<float> coefficients, uint32_t m_gain_bits);
    int32_t process_one();
    int32_t process_one_sym();
};

constexpr size_t MAX_CIC_ORDER{8};

class CICFilter final {
public:
    CICFilter(uint32_t order /* M */, uint32_t downsample/* R */) 
        : m_decimation_factor{downsample}, m_order(order) {  
        uint32_t n = 1;
        while (order--)
            n *= downsample;
        uint32_t actual_gain = n; 
        m_attenuate_shift = 32 - __builtin_clz(n) - 1;
        m_gain = (float)actual_gain / (1UL << m_attenuate_shift);
    }

    ~CICFilter() = default;

    void write(uint16_t *data, size_t length, size_t step = 1);
    size_t out_len() { return m_values.size(); }
    size_t read(uint16_t *out, size_t max_length);
    float gain() { return m_gain; }

    uint32_t overflow_cnt{0};
    uint32_t samples_out_cnt{0};
private:
    int32_t     m_int_delay[MAX_CIC_ORDER]{};
    int32_t     m_comb_delay[MAX_CIC_ORDER]{};
    uint32_t    m_attenuate_shift{1};
    float       m_gain;
    size_t      m_data_counter{0};
    size_t      m_decimation_factor{1};
    size_t      m_order{1};
    size_t      m_max_values{FILTER_OUTPUT_LEN};
    std::deque<uint32_t> m_values;
};

}