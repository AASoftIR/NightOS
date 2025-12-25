/*
 * NightOS - Shell Header
 * 
 * Basic command-line shell interface
 */

#ifndef SHELL_H
#define SHELL_H

#include "types.h"
#include "config.h"

/* Command handler function type */
typedef void (*command_handler_t)(int argc, char* argv[]);

/* Command structure */
typedef struct {
    const char* name;
    const char* description;
    command_handler_t handler;
} shell_command_t;

/* Initialize shell */
void shell_init(void);

/* Run shell main loop */
void shell_run(void);

/* Execute a command string */
void shell_execute(const char* input);

/* Register a command */
void shell_register_command(const char* name, const char* desc, command_handler_t handler);

/* Print shell prompt */
void shell_prompt(void);

/* Built-in commands */
void cmd_help(int argc, char* argv[]);
void cmd_clear(int argc, char* argv[]);
void cmd_echo(int argc, char* argv[]);
void cmd_version(int argc, char* argv[]);
void cmd_reboot(int argc, char* argv[]);
void cmd_halt(int argc, char* argv[]);
void cmd_time(int argc, char* argv[]);

#endif /* SHELL_H */
