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

// Combined processing unit:
// - threshold comparison (translation to [0,1])
// - object detection (consecutive series of 1's)
class ObjectDetector final {
public:
    ObjectDetector(int32_t threshold, uint32_t min_length) : 
        m_threshold{threshold}, m_min_length{min_length} {}

    void write(const int16_t *data, size_t length);
    uint64_t get_timestamp() { return m_timestamp; }

    std::deque<detected_object_t> results{};
private:
    // m_timestamp increases with each sample, providing timestamp for detected objects
    uint64_t m_timestamp{0};
    // Threshold for object detection
    int32_t m_threshold;
    uint32_t m_min_length;
    bool    m_in_obj{false};
    uint64_t m_obj_start{0};
    int32_t m_obj_power{0};
    int32_t m_obj_ampl{0};
};

}