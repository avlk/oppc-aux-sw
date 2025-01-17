#include <Arduino.h>
#include "cli_out.h"
#include <stdio.h>
#include <stdarg.h>

#define ENDL "\r\n"

/*
Device should write its output in strings ending with \r\n

Any string not starting with [ sign is considered as program random output and will not 
be parsed automatically.

Strings starting with [ are considered logging or info items in a machine-parseable format.
1. Debug - strings to be just logged. They should not carry machine-parseable information.
    [D] Any text
2. Command results. Each command should end with the result string. It should 
   be either OK, FAIL, or N/I (not implemented), and no other values.
    [R] OK
    [R] FAIL
    [R] N/I
3. Info strings. These are important info/measurement results output by the command when 
   run (before [R] string), that may be interpreted by some software on the other side of CLI.
    [I] Vcc 3.28
    [I] Tdelay 19
    [I] offset -57
4. CLI strings. Like debug strings, they can be about anything, but they are 
   output only by CLI interface itself.
    [ ] READY
    [ ] Running command `test`
 */

void cli_debug(const char* format, ...) {
    va_list arglist;

    printf("[D] ");
    va_start(arglist, format );
    vprintf(format, arglist );
    va_end(arglist);
    printf(ENDL);
}

void cli_info(const char* format, ...) {
    va_list arglist;

    printf("[I] ");
    va_start(arglist, format );
    vprintf(format, arglist );
    va_end(arglist);
    printf(ENDL);
}

void cli_aux_print(const char* format, ...) {
    va_list arglist;

    printf("[ ] ");
    va_start(arglist, format );
    vprintf(format, arglist );
    va_end(arglist);
    printf(ENDL);
}

void cli_result(cli_result_t res) {
    switch (res) {
    case CMD_OK: 
        printf("[R] OK" ENDL); 
        break;
    case CMD_ERROR: 
        printf("[R] FAIL" ENDL); 
        break;
    case CMD_NOT_IMPLEMENTED: 
        printf("[R] N/I" ENDL); 
        break;
    }
}

