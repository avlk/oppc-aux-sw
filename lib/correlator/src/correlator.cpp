#include <cstring>
#include "correlator.h"

using namespace correlator;

void CircularBuffer::write(uint16_t *buf, size_t length) {
    if (length > m_capacity) {
        // Store only latest data
        buf += (length - m_capacity);
        length = m_capacity;
    }

    if (m_data_ptr + length > m_capacity) {
        const size_t first_chunk = m_capacity - m_data_ptr;
        std::memcpy(m_data + m_data_ptr, buf, first_chunk*sizeof(uint16_t));
        std::memcpy(m_data, buf + first_chunk, (length - first_chunk)*sizeof(uint16_t));
        m_data_ptr = (length - first_chunk);
    } else {
        std::memcpy(m_data + m_data_ptr, buf, length*sizeof(uint16_t));
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
 *   1..3 pairs of data with pointers into a and b
 *   0 pairs on any error
 */
std::vector<processing_pair_t> 
correlator::break_chunks(const CircularBuffer &a,
             const CircularBuffer &b,
             int start_a, int start_b,
             int length) {
    std::vector<processing_pair_t> ret{};

    // check that a has all the data needed
    if ((start_a + length) > 0)
        return ret;
    if (start_a < -a.get_capacity())
        return ret;

    // get that b has all the data needed and get the chunks
    if ((start_b + length) > 0)
        return ret;
    auto chunks_b = b.get_data_chunks(start_b, length);
    if (chunks_b.size() == 0)
        return ret;

    int b_len{0};
    for (const auto &bc : chunks_b) {
        int a_offset = start_a + b_len;
        auto chunks_a = a.get_data_chunks(a_offset, bc.length);
        int a_len{0};
        for (const auto &ac : chunks_a) {
            processing_pair_t pair;
            pair.a = ac;
            pair.b.start = bc.start + a_len;
            pair.b.length = ac.length;
            ret.push_back(pair);
            a_len += ac.length; 
        }
        b_len += bc.length; 
    }

    return ret;
}

static uint64_t correlate_pair(const processing_pair_t& p) {
    uint64_t sum{0};
    uint16_t *pa{p.a.start};
    uint16_t *pb{p.b.start};
    size_t len{p.a.length};
    uint32_t subsum;

    // Calculate in blocks of 8 first
    while (len >= 8) {
        // With values not exceeding 13 bits, we can add 
        // up to 64 products without overflowing
        subsum  = pa[0]*pb[0];
        subsum += pa[1]*pb[1];
        subsum += pa[2]*pb[2];
        subsum += pa[3]*pb[3];
        subsum += pa[4]*pb[4];
        subsum += pa[5]*pb[5];
        subsum += pa[6]*pb[6];
        subsum += pa[7]*pb[7];
        len -= 8;
        pa += 8;
        pb += 8;
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


void correlator::correlate(const CircularBuffer &a,
                        const CircularBuffer &b,
                        uint32_t a_offset_min, 
                        uint32_t a_offset_max,
                        uint32_t length,
                        correlator::correlate_callback_t &callback) {
    int32_t max_index{0};
    uint64_t max_val{0};

    for (int offset = a_offset_min; offset < a_offset_max; offset++) {
        int start_a = -length - offset;
        int start_b = -length;
        auto pairs = break_chunks(a, b, start_a, start_b, length);
        uint64_t sum{0};
        for (const auto &c: pairs) {
            sum += correlate_pair(c);
        } 
        callback(offset, sum);
    }
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
