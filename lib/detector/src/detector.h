#pragma once
#include <stdint.h>
#include <vector>
#include <deque>

namespace detector {


typedef struct {
    uint64_t start;
    uint32_t len;
    int32_t  power;
    int32_t  ampl;
    uint32_t source;
} detected_object_t;

// DC filter pole would be (1 << DC_BASE_SHIFT - DC_POLE_NUM)/(1 << DC_BASE_SHIFT)
// e.g. (32768 - 4)/32768 = 0.999878
constexpr uint32_t DC_BASE_SHIFT{15};
constexpr uint32_t DC_POLE_NUM{4}; 

// Combined processing unit which includes, in series:
// - DC removal
// - threshold comparison (translation to [0,1])
// - median filtering of binary stream
// - object detection (consecutive series of 1's)
class ObjectDetector final {
public:
    ObjectDetector(int32_t threshold, uint32_t min_length) : 
        m_threshold{threshold}, m_min_length{min_length} {}

    void preinit(int16_t val) { m_dc_prev_x = val * (1 << DC_BASE_SHIFT); }
    void write(const int16_t *data, size_t length);
    uint64_t get_timestamp() { return m_timestamp; }

    std::deque<detected_object_t> results{};
private:
    // m_timestamp increases with each sample, providing timestamp for detected objects
    uint64_t m_timestamp{0};
    // DC removal internal data
    int32_t m_dc_acc{0};
    int32_t m_dc_prev_x{0};
    int32_t m_dc_prev_y{0};
    // Threshold for object detection
    int32_t m_threshold;
    uint32_t m_min_length;
    bool    m_in_obj{false};
    uint64_t m_obj_start{0};
    int32_t m_obj_power{0};
    int32_t m_obj_ampl{0};
};

}