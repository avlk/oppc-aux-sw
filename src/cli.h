#ifndef _CLI_H
#define _CLI_H

#include <stdlib.h>

typedef enum {
    CMD_OK                 = 0,
    CMD_ERROR              = 1,
    CMD_NOT_IMPLEMENTED    = 2
} cli_result_t;

typedef struct {
  cli_result_t (*cmd)(size_t argc, const char *argv[]);
  const char * name;
} cli_cmd_t; 

void cli_init(const cli_cmd_t *commands, size_t commands_size);
void cli_loop();

/* Returns 1/0 for true/false values, -1 for unknown words */
int onoff_to_bool(const char *onoff);

#endif
