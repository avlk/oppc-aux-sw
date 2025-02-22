#include <cstring>
#include "correlator.h"
// #include "pico/stdlib.h"
// #include "../../../src/cli_out.h"

using namespace correlator;


#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#ifndef PLATFORM_NATIVE
#define EXECUTE_FROM_RAM(subsection) __attribute__ ((long_call, section (".time_critical." subsection))) 
#else
#define EXECUTE_FROM_RAM(subsection)
#endif


void CircularBuffer::write(const int16_t *buf, size_t length) {
    if (length > m_capacity) {
        // Store only latest data
        buf += (length - m_capacity);
        length = m_capacity;
    }

    if (m_data_ptr + length > m_capacity) {
        const size_t first_chunk = m_capacity - m_data_ptr;
        std::memcpy(m_data + m_data_ptr, buf, first_chunk*sizeof(int16_t));
        std::memcpy(m_data, buf + first_chunk, (length - first_chunk)*sizeof(int16_t));
        m_data_ptr = (length - first_chunk);
    } else {
        std::memcpy(m_data + m_data_ptr, buf, length*sizeof(int16_t));
        m_data_ptr += length;
    }        
}


std::vector<processing_unit_t> CircularBuffer::get_data_chunks(int start, int length) const {
    std::vector<processing_unit_t> ret{};

    if ((length > m_capacity) || (start >= 0))
        return ret;
    if (-start > m_capacity)
        return ret;

    int ptr = m_data_ptr + start; // start is < 0
    if (ptr < 0)
        ptr += m_capacity;

    if (ptr + length > m_capacity) {
        const size_t first_chunk = m_capacity - ptr;
        ret.push_back({m_data + ptr, first_chunk});
        ret.push_back({m_data, length - first_chunk});
    } else {
        ret.push_back({m_data + ptr, static_cast<size_t>(length)});
    }        

    return ret;
}

size_t CircularBuffer::get_data_chunks_c(int start, int length, processing_unit_t out[2]) const {

    if ((length > m_capacity) || (start >= 0))
        return 0;
    if (-start > m_capacity)
        return 0;

    int ptr = m_data_ptr + start; // start is < 0
    if (ptr < 0)
        ptr += m_capacity;

    if (ptr + length > m_capacity) {
        const size_t first_chunk = m_capacity - ptr;
        out[0] = {m_data + ptr, first_chunk};
        out[1] = {m_data, length - first_chunk};
        return 2;
    } else {
        out[0] = {m_data + ptr, static_cast<size_t>(length)};
        return 1;
    }        
}



static size_t break_chunks_b(const CircularBuffer &b, int start_b,
                             int length, processing_unit_t chunks_b[2]) {

    // get that b has all the data needed and get the chunks
    if ((start_b + length) > 0)
        return 0;
    return b.get_data_chunks_c(start_b, length, chunks_b);
}

static size_t break_chunks_a(const CircularBuffer &a, int start_a, 
                            const processing_unit_t chunks_b[2], size_t n_chunks_b,
                            processing_pair_t pairs[4]) {
    int b_len{0};
    size_t n_out = 0;

    for (size_t n_b = 0; n_b < n_chunks_b; n_b++) {
        const auto *bc{&chunks_b[n_b]};

        int a_offset = start_a + b_len;
        processing_unit_t chunks_a[2];
        auto n_chunks_a = a.get_data_chunks_c(a_offset, bc->length, chunks_a);
        int a_len{0};
        for (size_t n_a = 0; n_a < n_chunks_a; n_a++) {
            pairs[n_out].a = chunks_a[n_a];
            pairs[n_out].b.start = bc->start + a_len;
            pairs[n_out].b.length = pairs[n_out].a.length;
            n_out++;
            a_len += chunks_a[n_a].length; 
        }
        b_len += bc->length; 
    }

    return n_out;
}

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
size_t correlator::break_chunks(const CircularBuffer &a,
                const CircularBuffer &b,
                int start_a, int start_b,
                int length,     
                processing_pair_t pairs[4]) {

    // check that a has all the data needed
    if ((start_a + length) > 0)
        return 0;
    if (start_a < -a.get_capacity())
        return 0;

    // get that b has all the data needed and get the chunks
    if ((start_b + length) > 0)
        return 0;

    processing_unit_t chunks_b[2];
    size_t n_chunks_b = break_chunks_b(b, start_b, length, chunks_b);
    if (!n_chunks_b)
        return 0;
    return break_chunks_a(a, start_a, chunks_b, n_chunks_b, pairs);
}

static uint64_t correlate_pair(const processing_pair_t& p) {
    uint64_t sum{0};
    const int16_t *pa{p.a.start};
    const int16_t *pb{p.b.start};
    uint32_t subsum;
    constexpr size_t batch_size{16};
    size_t len{p.a.length};
    
    // Calculate in blocks of batch_size first
    while (likely(len >= batch_size)) {
        // With values not exceeding 13 bits, we can add 
        // up to 64 products without overflowing

        // Compiler will unroll this loop for optimisation
        subsum = 0;
        #pragma GCC unroll 16
        for (size_t n = 0; n < batch_size; n++)
            subsum += pa[n] * pb[n];

        len -= batch_size;
        pa += batch_size;
        pb += batch_size;
        sum += subsum;
    }

    // Calculate the rest
    subsum = 0;
    while (len-- > 0) {
        // With values not exceeding 13 bits, we can add 
        // up to 64 products without overflowing
        subsum += (*pa++)* (*pb++);
    }
    sum += subsum;
    return sum;
}

EXECUTE_FROM_RAM("cor")
void correlator::correlate(const CircularBuffer &a,
                        const CircularBuffer &b,
                        uint32_t a_offset_min, 
                        uint32_t a_offset_max,
                        uint32_t length,
                        correlator::correlate_callback_t &callback) {
    int32_t max_index{0};
    uint64_t max_val{0};
    uint64_t time_chunks{0};
    uint64_t time_correlate{0};
    uint64_t n_correlations{0};
    processing_unit_t chunks_b[2];
    processing_pair_t pairs[4];
    const int start_b = -length;

    // auto start_c{time_us_64()};
    size_t n_chunks_b = break_chunks_b(b, start_b, length, chunks_b);
    // time_chunks += time_us_64() - start_c;

    for (int offset = a_offset_min; offset < a_offset_max; offset++) {
        int start_a = -length - offset;

        // auto start_c{time_us_64()};
        size_t n_pairs = break_chunks_a(a, start_a, 
                            chunks_b, n_chunks_b, pairs);
        // time_chunks += time_us_64() - start_c;

        uint64_t sum{0};
        // auto start_p{time_us_64()};
        for (size_t n = 0; n < n_pairs; n++) {
            sum += correlate_pair(pairs[n]);
            n_correlations += pairs[n].a.length;
        }
        // time_correlate += time_us_64() - start_p;
        callback(offset, sum);
    }

    // cli_info("chunks %llu", time_chunks);
    // cli_info("correlate_pair %llu", time_correlate);
    // cli_info("correlate_len %llu", n_correlations);
}

std::pair<int32_t, uint64_t> 
correlator::correlate_max(const CircularBuffer &a,
                        const CircularBuffer &b,
                        uint32_t a_offset_min, 
                        uint32_t a_offset_max,
                        uint32_t length) {
    int32_t max_index{0};
    uint64_t max_val{0};

    correlator::correlate_callback_t f([&](int32_t offset, uint64_t sum) {
        if (sum > max_val) {
            max_val = sum;
            max_index = offset;
        }
    }); 

    correlate(a, b, a_offset_min, a_offset_max, length, f);

    return std::make_pair(max_index, max_val);
}
