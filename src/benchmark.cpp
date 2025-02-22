#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include "adc.h"
#include "filter.h"
#include "correlator.h"
#include "cli.h"
#include "cli_out.h"
#include "benchmark.h"

// baseline
// benchmark 0
// [I] rounds 2000
// [I] time,ms 441
// [I] samples 256000
// [I] samples/s 580498
// benchmark 1
// [I] rounds 2000
// [I] time,ms 229
// [I] samples 256000
// [I] samples/s 1117903


// current result
// [I] benchmark 0
// [I] rounds 2000
// [I] time,ms 182
// [I] samples 256000
// [I] samples/s 1406593
// [I] benchmark 1
// [I] rounds 2000
// [I] time,ms 46
// [I] samples 256000
// [I] samples/s 5565217



void filter_benchmark_cic_cpp(size_t rounds, int stage) {
    filter::CICFilter</* M */4, /* R */5> filter_first;

    int16_t rx_buf[ADC_BUF_LEN];
    constexpr size_t out_buf_len{ADC_BUF_LEN};
    int16_t out_buf[out_buf_len];

    memset(rx_buf, 0, sizeof(rx_buf));
    rx_buf[0] = 1000;
    rx_buf[1] = 1000;
    rx_buf[32] = 1000;
    rx_buf[33] = 1000;
    rx_buf[64] = 1000;
    rx_buf[65] = 1000;

    while (rounds--) {
        size_t len;

        // .. process incoming stage

        filter_first.write(rx_buf, ADC_BUF_LEN);
        if (stage == 1)
            continue;

        // .. process second stage
        if (filter_first.out_len() > 64) {
            len = filter_first.read(out_buf, out_buf_len);
        }
    }
}

void filter_benchmark_cic_c(size_t rounds, int stage) {
    cic_filter_t filter_first;
    cic_init(&filter_first, /* M */4, /* R */5);

    uint16_t rx_buf[ADC_BUF_LEN];
    constexpr size_t out_buf_len{ADC_BUF_LEN};
    uint16_t out_buf[out_buf_len];

    memset(rx_buf, 0, sizeof(rx_buf));
    rx_buf[0] = 1000;
    rx_buf[1] = 1000;
    rx_buf[32] = 1000;
    rx_buf[33] = 1000;
    rx_buf[64] = 1000;
    rx_buf[65] = 1000;

    while (rounds--) {
        size_t len;

        // .. process incoming stage

        cic_write<4,5>(&filter_first, rx_buf, ADC_BUF_LEN, 1);
        if (stage == 1)
            continue;

        // .. process second stage
        if (filter_first.out_cnt > 64) {
            len = cic_read(&filter_first, out_buf, out_buf_len);
        }
    }
}

// baseline
// [I] benchmark FIR
// [I] stage 0
// [I] rounds 2000
// [I] time,ms 325
// [I] samples 256000
// [I] samples/s 787692
// [I] benchmark FIR
// [I] stage 1
// [I] rounds 2000
// [I] time,ms 312
// [I] samples 256000
// [I] samples/s 820512

// results
// [I] benchmark FIR
// [I] stage 0
// [I] rounds 2000
// [I] time,ms 217
// [I] samples 256000
// [I] samples/s 1179723
// [I] benchmark FIR
// [I] stage 1
// [I] rounds 2000
// [I] time,ms 211
// [I] samples 256000
// [I] samples/s 1213270

// FIR low pass filter 
// Fs 48kHz, 
// Passband 5000Hz
// Stopband 8000Hz
// Stopband attenuation 75dB
// length = 47
// Fout 16kHz after decimation=3 

static std::vector<float> fir_lp_48k_5k
{
     -0.000393, -0.000876, -0.001113, -0.000412,
     0.001640, 0.004554, 0.006716, 0.006048,
     0.001485, -0.005606, -0.011269, -0.010823,
     -0.002123, 0.011747, 0.022629, 0.021264,
     0.003557, -0.024493, -0.046941, -0.044427,
     -0.004546, 0.069449, 0.157022, 0.227790,
     0.254931, 0.227790, 0.157022, 0.069449,
     -0.004546, -0.044427, -0.046941, -0.024493,
     0.003557, 0.021264, 0.022629, 0.011747,
     -0.002123, -0.010823, -0.011269, -0.005606,
     0.001485, 0.006048, 0.006716, 0.004554,
     0.001640, -0.000412, -0.001113, -0.000876,
     -0.000393,
};

void filter_benchmark_fir(size_t rounds, int stage) {
    // Second-stage lowpass filters with passband 5kHz and decimation = 3
    // Has output rate of 16ksps
    filter::FIRFilter filter_second(fir_lp_48k_5k, 3);
    
    constexpr size_t out_buf_len{ADC_BUF_LEN};
    int16_t out_buf[out_buf_len];
    int16_t rx_buf[64];

    memset(rx_buf, 0, sizeof(rx_buf));
    rx_buf[0] = 1000;
    rx_buf[1] = 1000;
    rx_buf[32] = 1000;
    rx_buf[33] = 1000;

    while (rounds--) {
        filter_second.write(rx_buf, 64);

        if (filter_second.out_len() > 64) {
            if (stage == 1)
                filter_second.consume(filter_second.out_len());
            else
                filter_second.read(out_buf, out_buf_len);
        }
    }
}


void correlator_benchmark(size_t rounds, int stage) {
    constexpr unsigned int fs{16000};
    constexpr unsigned int ms{fs/1000};

    constexpr unsigned int offset_a_min{200*ms};
    constexpr unsigned int offset_a_max{350*ms};
    constexpr unsigned int buf_fill{700*ms};

    correlator::CircularBuffer buf_a(buf_fill); // 0.7s
    correlator::CircularBuffer buf_b(300*ms);  // 0.3s
    int16_t buf1[16];
    int16_t buf2[16];
    for (unsigned int n = 0; n < 16; n ++) {
        buf1[n] = 100;
        buf2[n] = 100;
    }
    buf2[3] = 125;
    buf2[4] = 150;
    buf2[5] = 700;
    buf2[6] = 150;
    buf2[7] = 125;
    constexpr unsigned int a_offset{buf_fill - 300*ms};
    constexpr unsigned int b_offset{buf_fill - 20*ms};

    // fill buffers with same values and a chirp positioned
    // at buf_a[-300ms], buf_b[-20ms]
    for (unsigned int n = 0; n < buf_fill; n += 16) {
        if (n == a_offset)
            buf_a.write(buf2, 16);
        else
            buf_a.write(buf1, 16);

        if (n == b_offset)
            buf_b.write(buf2, 16);
        else
            buf_b.write(buf1, 16);
    }

    while (rounds--) {
        auto ret = correlator::correlate_max(buf_a, buf_b, offset_a_min, offset_a_max, buf_b.get_capacity());
        // correlator_offset  = ret.first / ms;
    }
}

//#define CIC_C 1
void filter_benchmark(size_t rounds, int stage) {
    // First-stage lowpass filters with passband < 50kHz and decimation = 5
    // Has output rate of 50ksps
#ifndef CIC_C
    filter::CICFilter</* M */4, /* R */5> filter_first;
#else
    cic_filter_t filter_first;
    cic_init(&filter_first, /* M */4, /* R */5);
#endif

    // Second-stage lowpass filters with passband 5kHz and decimation = 3
    // Has output rate of 16ksps
    filter::FIRFilter filter_second(fir_lp_48k_5k, 3);

    int16_t rx_buf[ADC_BUF_LEN];
    constexpr size_t out_buf_len{ADC_BUF_LEN};
    int16_t out_buf[out_buf_len];

    memset(rx_buf, 0, sizeof(rx_buf));
    rx_buf[0] = 1000;
    rx_buf[1] = 1000;
    rx_buf[32] = 1000;
    rx_buf[33] = 1000;
    rx_buf[64] = 1000;
    rx_buf[65] = 1000;

    while (rounds--) {
        size_t len;

        // .. process incoming stage

#ifndef CIC_C
        filter_first.write(rx_buf, ADC_BUF_LEN);
#else
        cic_write<4,5>(&filter_first, rx_buf, ADC_BUF_LEN, 1);
#endif
        if (stage == 1)
            continue;

        // .. process second stage
#ifndef CIC_C
        if (filter_first.out_len() > 64) {
            len = filter_first.read(out_buf, out_buf_len);
#else
        if (filter_first.out_cnt > 64) {
            len = cic_read(&filter_first, out_buf, out_buf_len);
#endif
            if (stage == 2)
                continue;
            if (len > 0)
                filter_second.write(out_buf, len);
        }

        // .. consume second stage results
        if (filter_second.out_len() > 64) {
            int avg{0};
            if (stage == 3)
                filter_second.consume(filter_second.out_len());
            else
                filter_second.read(out_buf, out_buf_len);
        }
    }
}

// Make sure main CPU consumers (signal chain threads)
//  are disabled when running benchmarks
cli_result_t benchmark_cmd(size_t argc, const char *argv[]) {
    size_t n_rounds = 2000;
    int stage = 0;
    size_t samples_per_round = 128;
    void (* benchmark_func)(size_t n_rounds, int state) = filter_benchmark;
    const char *benchmark_name = "total";

    if (argc >= 1) {
        if (argc > 1) 
            stage = atoi(argv[1]);
    
        if (!strcmp(argv[0], "cic")) {
            benchmark_func = filter_benchmark_cic_cpp;
            benchmark_name = "CIC/C++";
        } else
        if (!strcmp(argv[0], "cicc")) {
            benchmark_func = filter_benchmark_cic_c;
            benchmark_name = "CIC/C";
        } else
        if (!strcmp(argv[0], "fir")) {
            benchmark_func = filter_benchmark_fir;
            benchmark_name = "FIR";
        } else
        if (!strcmp(argv[0], "cor")) {
            benchmark_func = correlator_benchmark;
            n_rounds = 1;
            samples_per_round = 1;
            benchmark_name = "COR";
        } else
        if (!strcmp(argv[0], "fir")) {
            benchmark_func = filter_benchmark;
            benchmark_name = "total";
        }
    }

    TickType_t start = xTaskGetTickCount();
    benchmark_func(n_rounds, stage);
    TickType_t stop = xTaskGetTickCount();

    size_t time_ms = (stop - start) * portTICK_PERIOD_MS;

    cli_info("benchmark %s", benchmark_name);
    cli_info("stage %d", stage);
    cli_info("rounds %d", n_rounds);
    cli_info("time,ms %d", time_ms);
    cli_info("samples %d", n_rounds * samples_per_round);
    cli_info("samples/s %d", (n_rounds * samples_per_round * 1000) / time_ms );

    return CMD_OK;
}
