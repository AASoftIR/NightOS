/*
 * NightOS - System Configuration
 * 
 * Central configuration file for OS parameters
 */

#ifndef CONFIG_H
#define CONFIG_H

/* OS Information */
#define OS_NAME     "NightOS"
#define OS_VERSION  "0.1.0"
#define OS_CODENAME "Midnight"

/* Memory Configuration */
#define KERNEL_HEAP_START 0x100000    /* 1MB */
#define KERNEL_HEAP_SIZE  0x100000    /* 1MB heap */
#define KERNEL_STACK_SIZE 0x4000      /* 16KB stack */

/* VGA Configuration */
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

/* Keyboard Configuration */
#define KEYBOARD_BUFFER_SIZE 256

/* Shell Configuration */
#define SHELL_MAX_INPUT     256
#define SHELL_MAX_ARGS      16
#define SHELL_PROMPT        "night> "

/* Color Theme (Dark Theme) */
#define THEME_BG            VGA_COLOR_BLACK
#define THEME_FG            VGA_COLOR_LIGHT_GREY
#define THEME_PROMPT        VGA_COLOR_CYAN
#define THEME_ERROR         VGA_COLOR_LIGHT_RED
#define THEME_SUCCESS       VGA_COLOR_LIGHT_GREEN
#define THEME_HIGHLIGHT     VGA_COLOR_WHITE

#endif /* CONFIG_H */
