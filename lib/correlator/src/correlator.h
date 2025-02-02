#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <utility>

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

    void write(uint16_t *buf, size_t length);

    // Returns 1 or 2 chunks for the contiguous region
    // of length <= m_capacity and starting at start (start < 0) 
    std::vector<processing_unit_t> get_data_chunks(int start, int length) const;
private:
    uint16_t *m_data;
    size_t   m_data_ptr{0};
    size_t   m_capacity;
};

std::vector<processing_pair_t> break_chunks(const CircularBuffer &a,
                                            const CircularBuffer &b,
                                            int start_a, int start_b,
                                            int length);

std::pair<int32_t, uint64_t> correlate(const CircularBuffer &a,
                                       const CircularBuffer &b,
                                       uint32_t a_offset_min, 
                                       uint32_t a_offset_max,
                                       uint32_t length);


};
