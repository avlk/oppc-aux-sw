#ifndef _CLI_OUT_H
#define _CLI_OUT_H
#include "cli.h"

void cli_aux_print(const char* format, ...);
void cli_debug(const char* format, ...);
void cli_info(const char* format, ...);
void cli_result(cli_result_t res);

// Define TEST_OUT_TRACE_EN in the .c file before including test_out.h to enable local trace functions
#ifdef TEST_OUT_TRACE_EN
#define cli_trace cli_debug
#else
#define cli_trace(...) do { } while (0)
#endif

#endif
