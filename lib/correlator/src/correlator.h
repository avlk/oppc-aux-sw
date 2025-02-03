#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <utility>
#include <functional>

namespace correlator {

typedef struct {
    uint16_t *start;
    size_t length;
} processing_unit_t;

typedef struct {
    processing_unit_t a;
    processing_unit_t b;
} processing_pair_t;

class CircularBuffer final {
public:
    CircularBuffer(size_t size) : m_capacity{size} {
        // TODO throw something if it fails
        m_data = new uint16_t[size];
    }

    ~CircularBuffer() {
        delete[] m_data;
    }

    const uint16_t *get_data() { return m_data; }
    size_t get_data_ptr() const { return m_data_ptr; }
    size_t get_capacity() const { return m_capacity; }

    void write(const uint16_t *buf, size_t length);

    // Returns 1 or 2 chunks for the contiguous region
    // of length <= m_capacity and starting at start (start < 0) 
    std::vector<processing_unit_t> get_data_chunks(int start, int length) const;
    // Same, but fills out and returns number of filled chunks
    size_t get_data_chunks_c(int start, int length, processing_unit_t out[2]) const;

private:
    uint16_t *m_data;
    size_t   m_data_ptr{0};
    size_t   m_capacity;
};


/* 
 * `break_chunks` gets references into Circular buffers
 *   a - longer one
 *   b - shorter one
 * and generates a set of overlapping chunk pairs for convolution
 * where
 *   start_a - start of the convolution region in a (<0)
 *   start_b - start of the convolution region in b (<0)
 *   length  - total length of the convolution region, in samples
 * Returns:
 *   number of pairs of data filled in `pairs`  
 *   fills pairs of data with pointers into a and b
 */
size_t break_chunks(const CircularBuffer &a, const CircularBuffer &b,
                int start_a, int start_b, int length,     
                processing_pair_t pairs[4]);

typedef std::function<void(int32_t, uint64_t)> correlate_callback_t;

/*
 * Performs correlation of buffers `a` and `b`, where `b` is treated as a needle,
 * and `a` as a haystack. Last `length` samples of `b` are correlated to `a`,
 * with offset range in a from `a_offset_min` to `a_offset_max` in the past, 
 * compared to `a`. Callback function is called with offset and correlation value
 * for each offset.
 */
void correlate(const CircularBuffer &a,
                const CircularBuffer &b,
                uint32_t a_offset_min, 
                uint32_t a_offset_max,
                uint32_t length,
                correlate_callback_t &callback);

/*
 * Performs correlation as in correlate(), but returns peak value offset and value
 */
std::pair<int32_t, uint64_t> correlate_max(const CircularBuffer &a,
                                       const CircularBuffer &b,
                                       uint32_t a_offset_min, 
                                       uint32_t a_offset_max,
                                       uint32_t length);


};
