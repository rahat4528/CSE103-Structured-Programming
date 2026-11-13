/**
 * CSE103-OS: Interactive Kernel Command Shell
 * Unified command line interface over VGA display and serial COM1 port.
 */

#ifndef CSE103_SHELL_H
#define CSE103_SHELL_H

#include "types.h"

#define SHELL_MAX_CMD_LEN 256
#define SHELL_MAX_ARGS    16

typedef void (*shell_cmd_func_t)(int argc, char *argv[]);

typedef struct {
    const char *name;
    const char *description;
    shell_cmd_func_t func;
} shell_command_t;

void shell_init(void);
void shell_update(void);
void shell_execute(char *cmd_line);

#endif /* CSE103_SHELL_H */
