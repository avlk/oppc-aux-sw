#include <unity.h>
#include "filter.h"
#include <vector>
#include <iostream>

// FIR low pass filter with Hamming window (fiiir.com)
// nominal sampling rate 1000Hz, 
// -3dB cutoff frequency 200Hz
// Transition bandwidth  200Hz
// length = 17
static std::vector<float> hamming_1000_200_200 {
    -0.001873,
    0.003077,
    0.010843,
    -0.000000,
    -0.040902,
    -0.044693,
    0.081012,
    0.292371,
    0.400330,
    0.292371,
    0.081012,
    -0.044693,
    -0.040902,
    -0.000000,
    0.010843,
    0.003077,
    -0.001873
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

static uint16_t input_one_level = 1024;
static std::vector<int32_t> expected_step_response;
void setUp(void) {
    // set up int32_t step response

    for (size_t n = 0; n < hamming_1000_200_200_step_response.size(); n++) {
        expected_step_response.push_back((hamming_1000_200_200_step_response[n] * input_one_level));
    }
}

void tearDown(void) {
    // clean stuff up here
}

static bool nearly_equal(int32_t a, int32_t b) {
    return abs(a - b) <= 1;
}

static void check_response(int32_t out[17]) {
    size_t n;
#if 0
    for (n = 0; n < 17; n++) {
        char buf[128];
        sprintf(buf, "Sample[%d] = %d (expected %d)", n, out[n], expected_step_response[n]);
        TEST_MESSAGE(buf);
    }
#endif
    for (n = 0; n < 17; n++) {
        // We have our reference step response with this precision only
        if (!nearly_equal(expected_step_response[n], out[n])) {
            TEST_ASSERT_EQUAL_FLOAT(expected_step_response[n], out[n]);
        }
    }
}

void test_fir_filter_transfer_func(const std::vector<float> &coeff, bool symm,
                                   size_t start_buffer_position) {
    filter::FIRFilter filter(coeff, 1);
    uint16_t data[32];
    size_t n;
    int32_t out[17];

#if 0
    for (n = 0; n < hamming_1000_200_200.size(); n++) {
        char buf[128];
        sprintf(buf, "Filter[%d] = %f", n, hamming_1000_200_200[n]);
        TEST_MESSAGE(buf);
    }
#endif

    TEST_ASSERT_EQUAL(symm, filter.is_symmetric()); 

    // Push some zeros into the and read back the output,
    // so that the buffer fill is set at some level (and wrap)
    // before we start the test
    while (start_buffer_position--) {
        data[0] = 0;
        filter.write(data, 0);
        while (filter.out_len())
            filter.read(out, 1);
    }
    
    // pass 16 zeroes
    for (n = 0; n < 16; n++) 
        data[n] = 0;
    filter.write(data, 16);
    // Drain the output
    while (filter.out_len())
        filter.read(out, 1);

    // Pass 17 ones - it is a step function and generates 17 output values
    for (n = 0; n < 17; n++) 
        data[n] = input_one_level;
    filter.write(data, 17);
    // Check the output
    TEST_ASSERT_EQUAL_INT(17, filter.out_len()); 

    TEST_ASSERT_EQUAL_INT(17, filter.read(out, 17));

    check_response(out);
}

void test_fir_filter_symmetric() {
    test_fir_filter_transfer_func(hamming_1000_200_200, true, 0);
    test_fir_filter_transfer_func(hamming_1000_200_200, true, 10);
    test_fir_filter_transfer_func(hamming_1000_200_200, true, filter::FILTER_BUFFER_SIZE/2 - 11);
    test_fir_filter_transfer_func(hamming_1000_200_200, true, filter::FILTER_BUFFER_SIZE - 1);
}

void test_fir_filter_asymmetric() {
    auto coeff = hamming_1000_200_200;
    // Slightly change coefficients so that it's not symmetric
    coeff[0] += 0.001f; 
    coeff[1] -= 0.001f; 
    test_fir_filter_transfer_func(coeff, false, 0);
    test_fir_filter_transfer_func(coeff, false, 10);
    test_fir_filter_transfer_func(coeff, false, filter::FILTER_BUFFER_SIZE/2 - 11);
    test_fir_filter_transfer_func(coeff, false, filter::FILTER_BUFFER_SIZE/2 - 1);
}

void test_fir_filter_interleave() {
    filter::FIRFilter filter(hamming_1000_200_200, 1);
    uint16_t data[66] = {0};
    size_t n;

    for (n = 0; n < 16; n++) // 16 zeroes
        data[n*2+1] = 0;
    for (n = 16; n < 33; n++) // then 17 ones - generates 17 outputs
        data[n*2+1] = input_one_level;

    filter.write(data + 1, 66, 2);

    TEST_ASSERT_EQUAL_INT(17, filter.out_len()); 

    int32_t out[17];
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
        data[n] = input_one_level;
    filter.write(data, 32); // will generate 32/4=8 samples each
    filter.write(data, 32);

    TEST_ASSERT_EQUAL_INT(16, filter.out_len());

    int32_t out[16] = {0};
    TEST_ASSERT_EQUAL_INT(16, filter.read(out, 16));

    // [samples for which data is output]
    // 0  1  2  3
    // 4  5  6  7
    // 8  9  10 11
    // 12 13 14 15
    // [16] 17 18 19
    // [20] 21 22 23

    // With interleaving get samples 0, 4, 8, 16, and then ones
    TEST_ASSERT_TRUE(nearly_equal(out[0], expected_step_response[0]));
    TEST_ASSERT_TRUE(nearly_equal(out[1], expected_step_response[4]));
    TEST_ASSERT_TRUE(nearly_equal(out[2], expected_step_response[8]));
    TEST_ASSERT_TRUE(nearly_equal(out[3], expected_step_response[12]));
    TEST_ASSERT_TRUE(nearly_equal(out[4], input_one_level));
}

static std::vector<float> cic_impulse_response_M4_R5 = {
    /* 0.0016, 0.0064, 0.016,  0.032, */  0.056, 
    /* 0.0832, 0.1088, 0.128,  0.136, */  0.128,
    /* 0.1088, 0.0832, 0.056,  0.032, */  0.016,
    /* 0.0064, 0.0016, 0,  0, */  0,
};


void test_cic_filter_response() {
    filter::CICFilter filter(4, 5);
    uint16_t data[20] = {0};
    size_t n;

    TEST_ASSERT_EQUAL_FLOAT(625.0/512.0, filter.gain());

    data[0] = 8192;
    auto total_gain = data[0]*filter.gain();

    filter.write(data, 20);

    TEST_ASSERT_EQUAL_INT(4, filter.out_len());

    uint16_t out[4] = {0};
    TEST_ASSERT_EQUAL_INT(4, filter.read(out, 4));

#if 0
    for (n = 0; n < 4; n++) {
        char buf[128];
        sprintf(buf, "Filter[%d] = %d", n, (int)out[n]);
        TEST_MESSAGE(buf);
    }
#endif
    for (n = 0; n < 4; n++) {
        TEST_ASSERT_TRUE(out[n] == (uint16_t)(total_gain*cic_impulse_response_M4_R5[n]));
    }
}

void test_cic_filter_response_c() {
    cic_filter_t filter;
    cic_init(&filter, 4, 5);

    uint16_t data[20] = {0};
    size_t n;

    TEST_ASSERT_EQUAL_FLOAT(625.0/512.0, filter.gain);

    data[0] = 8192;
    auto total_gain = data[0]*filter.gain;

    cic_write(&filter, data, 20, 1);

    TEST_ASSERT_EQUAL_INT(4, filter.out_cnt);

    uint16_t out[4] = {0};
    TEST_ASSERT_EQUAL_INT(4, cic_read(&filter, out, 4));

#if 0
    for (n = 0; n < 4; n++) {
        char buf[128];
        sprintf(buf, "Filter[%d] = %d", n, (int)out[n]);
        TEST_MESSAGE(buf);
    }
#endif
    for (n = 0; n < 4; n++) {
        TEST_ASSERT_TRUE(out[n] == (uint16_t)(total_gain*cic_impulse_response_M4_R5[n]));
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_fir_filter_symmetric);
    RUN_TEST(test_fir_filter_asymmetric);
    RUN_TEST(test_fir_filter_interleave);
    RUN_TEST(test_fir_filter_decimate);
    RUN_TEST(test_cic_filter_response);
    RUN_TEST(test_cic_filter_response_c);

    UNITY_END();
}
