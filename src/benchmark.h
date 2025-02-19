#ifndef _BENCHMARK_H
#define _BENCHMARK_H

#include <stdlib.h>
#include "cli.h"

void filter_benchmark(size_t rounds, int stage);
void filter_benchmark_cic_cpp(size_t rounds, int stage);
void filter_benchmark_cic_c(size_t rounds, int stage);
void filter_benchmark_fir(size_t rounds, int stage);
void correlator_benchmark(size_t rounds, int stage);

cli_result_t benchmark_cmd(size_t argc, const char *argv[]);

#endif
