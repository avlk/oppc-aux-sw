#include <unity.h>
#include <string.h>
#include <stdio.h>
#include "correlator.h"

using namespace correlator;

void setUp(void) {
}

void tearDown(void) {
}

void test_circular_buffer_fill() {
    CircularBuffer cbuf(128);
    uint16_t data[64];

    TEST_ASSERT_EQUAL(0, cbuf.get_data_ptr());
    cbuf.write(data, 16);
    TEST_ASSERT_EQUAL(16, cbuf.get_data_ptr());
    cbuf.write(data, 64);
    TEST_ASSERT_EQUAL(80, cbuf.get_data_ptr());
    cbuf.write(data, 64);
    TEST_ASSERT_EQUAL(16, cbuf.get_data_ptr());
}

void test_circular_buffer_chunks() {
    CircularBuffer cbuf(128);
    uint16_t data[64];

    cbuf.write(data, 16);
    TEST_ASSERT_EQUAL(16, cbuf.get_data_ptr());

    std::vector<processing_unit_t> ret;
    auto buf = cbuf.get_data();

    ret = cbuf.get_data_chunks(-128, 128);
    TEST_ASSERT_EQUAL_INT(2, ret.size());
    TEST_ASSERT_EQUAL_PTR(buf + 16, ret[0].start);
    TEST_ASSERT_EQUAL_INT(112, ret[0].length);
    TEST_ASSERT_EQUAL_PTR(buf + 0, ret[1].start);
    TEST_ASSERT_EQUAL_INT(16, ret[1].length);

    ret = cbuf.get_data_chunks(-16, 16);
    TEST_ASSERT_EQUAL_INT(1, ret.size());
    TEST_ASSERT_EQUAL_PTR(buf + 0, ret[0].start);
    TEST_ASSERT_EQUAL_INT(16, ret[0].length);

    ret = cbuf.get_data_chunks(-32, 16);
    TEST_ASSERT_EQUAL_INT(1, ret.size());
    TEST_ASSERT_EQUAL_PTR(buf + 112, ret[0].start);
    TEST_ASSERT_EQUAL_INT(16, ret[0].length);

    ret = cbuf.get_data_chunks(-31, 16);
    TEST_ASSERT_EQUAL_INT(2, ret.size());
    TEST_ASSERT_EQUAL_PTR(buf + 113, ret[0].start);
    TEST_ASSERT_EQUAL_INT(15, ret[0].length);
    TEST_ASSERT_EQUAL_PTR(buf + 0, ret[1].start);
    TEST_ASSERT_EQUAL_INT(1, ret[1].length);

    ret = cbuf.get_data_chunks(0, 16);
    TEST_ASSERT_EQUAL_INT(0, ret.size());

    ret = cbuf.get_data_chunks(-129, 16);
    TEST_ASSERT_EQUAL_INT(0, ret.size());
}

void test_break_chunks() {
    CircularBuffer a(64);
    CircularBuffer b(16);
    uint16_t data[64];
    // Set a (larger) at 50% fill, position 32
    // Set b (smaller) at 75% fill, position 12

    a.write(data, 32);
    TEST_ASSERT_EQUAL(32, a.get_data_ptr());
    b.write(data, 12);
    TEST_ASSERT_EQUAL(12, b.get_data_ptr());

    auto abuf = a.get_data();
    auto bbuf = b.get_data();
    
    // Shall return single pair
    correlator::processing_pair_t ret[4];
    size_t ret_size;
    ret_size = break_chunks(a, b, -12, -12, 12, ret);
    TEST_ASSERT_EQUAL_INT(1, ret_size);
    TEST_ASSERT_EQUAL_PTR(abuf + 20, ret[0].a.start);
    TEST_ASSERT_EQUAL_INT(12, ret[0].a.length);
    TEST_ASSERT_EQUAL_PTR(bbuf + 0, ret[0].b.start);
    TEST_ASSERT_EQUAL_INT(12, ret[0].b.length);

    // Shall return two pairs because `a` would overlap
    ret_size = break_chunks(a, b, -40, -12, 12, ret);
    TEST_ASSERT_EQUAL_INT(2, ret_size);
    TEST_ASSERT_EQUAL_PTR(abuf + (64-8), ret[0].a.start);
    TEST_ASSERT_EQUAL_INT(8, ret[0].a.length);
    TEST_ASSERT_EQUAL_PTR(bbuf + 0, ret[0].b.start);
    TEST_ASSERT_EQUAL_INT(8, ret[0].b.length);
    TEST_ASSERT_EQUAL_PTR(abuf + 0, ret[1].a.start);
    TEST_ASSERT_EQUAL_INT(4, ret[1].a.length);
    TEST_ASSERT_EQUAL_PTR(bbuf + 8, ret[1].b.start);
    TEST_ASSERT_EQUAL_INT(4, ret[1].b.length);

    // Shall return two pairs because `b` would overlap
    ret_size = break_chunks(a, b, -48, -16, 12, ret);
    TEST_ASSERT_EQUAL_INT(2, ret_size);
    TEST_ASSERT_EQUAL_PTR(abuf + 48, ret[0].a.start);
    TEST_ASSERT_EQUAL_INT(4, ret[0].a.length);
    TEST_ASSERT_EQUAL_PTR(bbuf + (16-4), ret[0].b.start);
    TEST_ASSERT_EQUAL_INT(4, ret[0].b.length);
    TEST_ASSERT_EQUAL_PTR(abuf + 52, ret[1].a.start);
    TEST_ASSERT_EQUAL_INT(8, ret[1].a.length);
    TEST_ASSERT_EQUAL_PTR(bbuf + 0, ret[1].b.start);
    TEST_ASSERT_EQUAL_INT(8, ret[1].b.length);

    // Shall return 3 pairs because `a` and `b` would overlap
    ret_size = break_chunks(a, b, -44, -16, 16, ret);
    // a overlap: 12 + 4
    // b overlap: 4 + 12
    // summary 4 + 8 + 4
    TEST_ASSERT_EQUAL_INT(3, ret_size);
    TEST_ASSERT_EQUAL_PTR(abuf + 52, ret[0].a.start);
    TEST_ASSERT_EQUAL_INT(4, ret[0].a.length);
    TEST_ASSERT_EQUAL_PTR(bbuf + (16-4), ret[0].b.start);
    TEST_ASSERT_EQUAL_INT(4, ret[0].b.length);
    TEST_ASSERT_EQUAL_PTR(abuf + 56, ret[1].a.start);
    TEST_ASSERT_EQUAL_INT(8, ret[1].a.length);
    TEST_ASSERT_EQUAL_PTR(bbuf + 0, ret[1].b.start);
    TEST_ASSERT_EQUAL_INT(8, ret[1].b.length);
    TEST_ASSERT_EQUAL_PTR(abuf + 0, ret[2].a.start);
    TEST_ASSERT_EQUAL_INT(4, ret[2].a.length);
    TEST_ASSERT_EQUAL_PTR(bbuf + 8, ret[2].b.start);
    TEST_ASSERT_EQUAL_INT(4, ret[2].b.length);
}


void test_correlator() {
    CircularBuffer a(64);
    CircularBuffer b(16);
    uint16_t data[64];
    size_t n;

    for (n = 0; n < 64; n++)
        data[n] = 128 + rand() % 16;
    data[32] += 30;
    data[33] += 20;
    data[34] += 30;
    data[35] += 20;
    a.write(data, 64);

    for (n = 0; n < 64; n++)
        data[n] = 128 + rand() % 16;
    data[0] += 30;
    data[1] += 20;
    data[2] += 30;
    data[3] += 20;

    b.write(data, 16);

    auto ret = correlate_max(a, b, 10, 50, b.get_capacity());

    correlator::correlate_callback_t f{[&](int32_t offset, uint64_t val) {
        char buf[128];
        float mag = val;
        sprintf(buf, "[%d] -> %f", offset, mag);
        TEST_MESSAGE(buf);
    }};
    // Print correlation values
    correlate(a, b, 10, 50, b.get_capacity(), f);

    TEST_ASSERT_EQUAL(16, ret.first);

}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_circular_buffer_fill);
    RUN_TEST(test_circular_buffer_chunks);
    RUN_TEST(test_break_chunks);
    RUN_TEST(test_correlator);
 
    UNITY_END();
}
