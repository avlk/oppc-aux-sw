#include <Arduino.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cli_out.h"
#include "cli.h"

#define CMD_MAX_ARGS 8
#define CMD_BUF_LEN  256

static char *cli_get_command(bool echo);

cli_result_t interactive_cmd(size_t argc, const char *argv[]);
cli_result_t help_cmd(size_t argc, const char *argv[]);

static bool interactive_mode = false;
static const cli_cmd_t *cmd_list = NULL;
static int cmd_num = 0;

static const cli_cmd_t int_cmd_list[] = {
    {interactive_cmd, "*"},
    {help_cmd, "?"},
};
static int int_cmd_num = sizeof(int_cmd_list)/sizeof(int_cmd_list[0]);

void cli_init(const cli_cmd_t *commands, size_t commands_size) {
    cmd_list = commands;
    cmd_num = commands_size;
}

static void cli_prompt(void) {
    if (interactive_mode) {
        printf("> ");
        fflush(stdout);
    } else
        cli_aux_print("READY");
} 

void cli_loop() {
    cli_result_t (*cmd)(size_t argc, const char *argv[]);
    char *testname;

    char *cmd_buf = cli_get_command(interactive_mode);
    if (!cmd_buf)
        return;

    testname = strtok(cmd_buf, " ");
    if (!testname) {
        cli_prompt();
        return;
    }

    cmd = NULL;
    bool int_cmd = false;
    for (int i = 0; i < cmd_num; i++)
        if (!strcasecmp(cmd_list[i].name, testname)) {
            cmd = cmd_list[i].cmd;
            break;
        }

    if (cmd == NULL) {
        for (int i = 0; i < int_cmd_num; i++)
            if (!strcasecmp(int_cmd_list[i].name, testname)) {
                cmd = int_cmd_list[i].cmd;
                int_cmd = true;
                break;
            }
    }

    if (cmd == NULL) {
        cli_debug("Unknown command '%s'", testname);
        cli_debug("'*' to enter interactive mode");
        cli_debug("'?' for help");
        cli_result(CMD_NOT_IMPLEMENTED);
        cli_prompt();
        return;
    }

    const char *argv[CMD_MAX_ARGS];
    size_t argc = 0;

    // Fill in args
    while (argc < CMD_MAX_ARGS) {
        const char *arg = strtok(NULL, " ");
        if (arg)
            argv[argc++] = arg;
        else
            break;
    }

    if (int_cmd) {
        cmd(argc, argv);
    } else {
        cli_aux_print("Running cmd %s", testname);
        cli_result_t ret = cmd(argc, argv);
        cli_result(ret);
    }
    cli_prompt();
}


static char *cli_get_command(bool echo) {
    int c = -1;
    static size_t len = 0;
    static char buf[CMD_BUF_LEN];
    static size_t max_data_len = sizeof(buf) - 1;

    while (len < max_data_len) {
        if (Serial.available() < 1)
            return NULL;
        c = getchar();
        if (c < 0)
            return NULL;

        if (c == '\r' || c == '\n')
            break;

        if (c == '\b' && len) {
            if (echo)
                Serial.write("\b \b");
            len--;
            continue;
        }

        buf[len++] = c;
        if (echo)
            Serial.write(c);
    }

  buf[len] = 0;
  len = 0;
  if (echo)
    Serial.write("\r\n");
  return buf;
}


cli_result_t interactive_cmd(size_t argc, const char *argv[]) {
    cli_debug("Interactive mode ON");
    interactive_mode = true;
    return CMD_OK;
}

cli_result_t help_cmd(size_t argc, const char *argv[]) {
    for (int i = 0; i < cmd_num; i++)
        cli_aux_print(cmd_list[i].name);
    return CMD_OK;
}

int onoff_to_bool(const char *onoff) {
    if (!strcasecmp("on", onoff) ||
        !strcasecmp("1", onoff) ||
        !strcasecmp("true", onoff))
        return 1;

    if (!strcasecmp("off", onoff) ||
        !strcasecmp("0", onoff) ||
        !strcasecmp("false", onoff))
        return 0;
    return -1;
}
