#include <unity.h>
#include "filter.h"
#include <vector>
#include <iostream>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

// FIR low pass filter with Hamming window (fiiir.com)
// nominal sampling rate 1000Hz, 
// -3dB cutoff frequency 200Hz
// Transition bandwidth  200Hz
// length = 17
static std::vector<float> hamming_1000_200_200 {
    -0.001872521167090275,
    0.003076697131615380,
    0.010843204057061970,
    -0.000000000000000006,
    -0.040902339055576782,
    -0.044692984361066959,
    0.081011737060229752,
    0.292371308720313250,
    0.400329795229027119,
    0.292371308720313305,
    0.081011737060229766,
    -0.044692984361066980,
    -0.040902339055576789,
    -0.000000000000000006,
    0.010843204057061974,
    0.003076697131615383,
    -0.001872521167090275,
};

static std::vector<float> hamming_1000_200_200_step_response {
    -0.002,
    0.001,
    0.012,
    0.012,
    -0.029,
    -0.074,
    0.007,
    0.3,
    0.7,
    0.993,
    1.074,
    1.029,
    0.988,
    0.988,
    0.999,
    1.002,
    1.0
};

static bool f_nearly_equal(float a, float b) {
    return fabs(a - b) < 0.0005;
}

static void check_response(float out[17], int step = 1) {
    size_t n;
#if 0
    for (n = 0; n < 17; n += step) {
        char buf[128];
        sprintf(buf, "Sample[%d] = %.3f (expected %.3f)", n, out[n], hamming_1000_200_200_step_response[n]);
        TEST_MESSAGE(buf);
    }
#endif
    for (n = 0; n < 17; n += step) {
        // We have our reference step response with this precision only
        if (!f_nearly_equal(hamming_1000_200_200_step_response[n], out[n])) {
            TEST_ASSERT_EQUAL_FLOAT(hamming_1000_200_200_step_response[n], out[n]);
        }
    }
}

void test_fir_filter_single() {
    filter::FIRFilter filter(hamming_1000_200_200, 1);
    uint16_t data[33];
    size_t n;

    for (n = 0; n < 16; n++) // 16 zeroes
        data[n] = 0;
    for (n = 16; n < 33; n++) // then 17 ones - generates 17 outputs
        data[n] = 1;

    filter.write(data, 33);

    TEST_ASSERT_EQUAL_INT(17, filter.out_len()); 

    float out[17];
    TEST_ASSERT_EQUAL_INT(17, filter.read(out, 17));

    check_response(out);
}

void test_fir_filter_interleave() {
    filter::FIRFilter filter(hamming_1000_200_200, 1);
    uint16_t data[66] = {0};
    size_t n;

    for (n = 0; n < 16; n++) // 16 zeroes
        data[n*2+1] = 0;
    for (n = 16; n < 33; n++) // then 17 ones - generates 17 outputs
        data[n*2+1] = 1;

    filter.write(data + 1, 66, 2);

    TEST_ASSERT_EQUAL_INT(17, filter.out_len()); 

    float out[17];
    TEST_ASSERT_EQUAL_INT(17, filter.read(out, 17));

    check_response(out);
}

void test_fir_filter_decimate() {
    filter::FIRFilter filter(hamming_1000_200_200, 4);
    uint16_t data[32] = {0}; 
    size_t n;

    for (n = 0; n < 16; n++) // 16 zeroes
        data[n] = 0;
    filter.write(data, 16); // will be completely skipped on the output

    for (n = 0; n < 32; n++) // 32 ones
        data[n] = 1;
    filter.write(data, 32); // will generate 32/4=8 samples each
    filter.write(data, 32);

    TEST_ASSERT_EQUAL_INT(16, filter.out_len());

    float out[16] = {0};
    TEST_ASSERT_EQUAL_INT(16, filter.read(out, 16));

    // [samples for which data is output]
    // 0  1  2  3
    // 4  5  6  7
    // 8  9  10 11
    // 12 13 14 15
    // [16] 17 18 19
    // [20] 21 22 23

    // With interleaving get samples 0, 4, 8, 16, and then ones
    TEST_ASSERT_TRUE(f_nearly_equal(out[0], hamming_1000_200_200_step_response[0]));
    TEST_ASSERT_TRUE(f_nearly_equal(out[1], hamming_1000_200_200_step_response[4]));
    TEST_ASSERT_TRUE(f_nearly_equal(out[2], hamming_1000_200_200_step_response[8]));
    TEST_ASSERT_TRUE(f_nearly_equal(out[3], hamming_1000_200_200_step_response[12]));
    TEST_ASSERT_TRUE(f_nearly_equal(out[4], 1.0f));
}


int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_fir_filter_single);
    RUN_TEST(test_fir_filter_interleave);
    RUN_TEST(test_fir_filter_decimate);

    UNITY_END();
}
