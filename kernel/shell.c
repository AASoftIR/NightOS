/*
 * NightOS - Shell Implementation
 * 
 * Dark themed minimal shell inspired by Unix/Bash
 */

#include "../include/shell.h"
#include "../include/vga.h"
#include "../include/keyboard.h"
#include "../include/string.h"
#include "../include/io.h"
#include "../include/config.h"
#include "../include/timer.h"
#include "../include/rtc.h"
#include "../include/memory.h"
#include "../include/tui.h"

/* Maximum number of registered commands */
#define MAX_COMMANDS 32

/* Command registry */
static shell_command_t commands[MAX_COMMANDS];
static int num_commands = 0;

/* Input buffer */
static char input_buffer[SHELL_MAX_INPUT];
static int input_pos = 0;

/* Register a command */
void shell_register_command(const char* name, const char* desc, command_handler_t handler) {
    if (num_commands >= MAX_COMMANDS) {
        return;
    }
    commands[num_commands].name = name;
    commands[num_commands].description = desc;
    commands[num_commands].handler = handler;
    num_commands++;
}

/* Parse input into argc/argv */
static int parse_args(char* input, char* argv[], int max_args) {
    int argc = 0;
    char* token = input;
    bool in_token = false;
    
    while (*input && argc < max_args) {
        if (*input == ' ' || *input == '\t') {
            if (in_token) {
                *input = '\0';
                in_token = false;
            }
        } else {
            if (!in_token) {
                token = input;
                argv[argc++] = token;
                in_token = true;
            }
        }
        input++;
    }
    
    return argc;
}

/* Execute a command string */
void shell_execute(const char* input) {
    char buffer[SHELL_MAX_INPUT];
    char* argv[SHELL_MAX_ARGS];
    
    /* Copy input to modifiable buffer */
    strncpy(buffer, input, SHELL_MAX_INPUT - 1);
    buffer[SHELL_MAX_INPUT - 1] = '\0';
    
    /* Parse arguments */
    int argc = parse_args(buffer, argv, SHELL_MAX_ARGS);
    
    if (argc == 0) {
        return;
    }
    
    /* Find and execute command */
    for (int i = 0; i < num_commands; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            commands[i].handler(argc, argv);
            return;
        }
    }
    
    /* Command not found */
    vga_set_color(vga_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    vga_printf("Unknown command: %s\n", argv[0]);
    vga_set_color(vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    vga_puts("Type 'help' for available commands.\n");
}

/* Print shell prompt */
void shell_prompt(void) {
    vga_set_color(vga_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK));
    vga_puts(SHELL_PROMPT);
    vga_set_color(vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
}

/* Built-in: help */
void cmd_help(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    
    vga_set_color(vga_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    vga_puts("\n  NightOS Commands\n");
    vga_puts("  ================\n\n");
    vga_set_color(vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    
    for (int i = 0; i < num_commands; i++) {
        vga_set_color(vga_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
        vga_printf("  %-10s", commands[i].name);
        vga_set_color(vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
        vga_printf(" - %s\n", commands[i].description);
    }
    vga_putchar('\n');
}

/* Built-in: clear */
void cmd_clear(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    vga_clear();
}

/* Built-in: echo */
void cmd_echo(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        vga_puts(argv[i]);
        if (i < argc - 1) {
            vga_putchar(' ');
        }
    }
    vga_putchar('\n');
}

/* Built-in: version */
void cmd_version(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    
    vga_set_color(vga_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK));
    vga_printf("\n  %s ", OS_NAME);
    vga_set_color(vga_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    vga_printf("v%s", OS_VERSION);
    vga_set_color(vga_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK));
    vga_printf(" (%s)\n", OS_CODENAME);
    vga_set_color(vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    vga_puts("  A minimal dark-themed operating system\n");
    vga_puts("  Inspired by Unix/Linux with a modern approach\n\n");
}

/* Built-in: reboot */
void cmd_reboot(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    
    vga_set_color(vga_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK));
    vga_puts("Rebooting system...\n");
    
    /* Use keyboard controller to trigger reboot */
    uint8_t good = 0x02;
    while (good & 0x02) {
        good = inb(0x64);
    }
    outb(0x64, 0xFE);
    
    /* If that didn't work, halt */
    __asm__ volatile("cli; hlt");
}

/* Built-in: halt */
void cmd_halt(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    
    vga_set_color(vga_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK));
    vga_puts("\nSystem halted. It is safe to power off.\n");
    
    /* Disable interrupts and halt */
    __asm__ volatile("cli; hlt");
}

/* Built-in: time - now with RTC support */
void cmd_time(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    
    rtc_time_t time;
    char time_str[12];
    char date_str[12];
    
    rtc_read_time(&time);
    rtc_format_time(&time, time_str);
    rtc_format_date(&time, date_str);
    
    vga_set_color(vga_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    vga_puts("\n  Current Time: ");
    vga_set_color(vga_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK));
    vga_puts(time_str);
    
    vga_set_color(vga_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    vga_puts("\n  Current Date: ");
    vga_set_color(vga_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK));
    vga_puts(date_str);
    
    vga_set_color(vga_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    vga_puts("\n  Day: ");
    vga_set_color(vga_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK));
    vga_puts(rtc_day_name(time.weekday));
    
    vga_set_color(vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    vga_puts("\n\n");
}

/* Built-in: uptime */
void cmd_uptime(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    
    uint32_t seconds = timer_get_seconds();
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    
    vga_set_color(vga_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    vga_puts("\n  System Uptime: ");
    vga_set_color(vga_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK));
    vga_printf("%d:%02d:%02d", hours, minutes % 60, seconds % 60);
    vga_set_color(vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    vga_printf(" (Total ticks: %d)\n\n", timer_get_ticks());
}

/* Built-in: mem - show memory info */
void cmd_mem(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    
    memory_stats_t stats;
    memory_get_stats(&stats);
    
    vga_set_color(vga_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    vga_puts("\n  Memory Statistics\n");
    vga_puts("  =================\n");
    
    vga_set_color(vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    vga_printf("  Total Memory:  %d KB\n", stats.total_memory / 1024);
    vga_printf("  Used Memory:   %d KB\n", stats.used_memory / 1024);
    vga_printf("  Free Memory:   %d KB\n", stats.free_memory / 1024);
    vga_printf("  Allocations:   %d\n", stats.allocations);
    vga_printf("  Frees:         %d\n\n", stats.frees);
}

/* Built-in: sleep */
void cmd_sleep(int argc, char* argv[]) {
    if (argc < 2) {
        vga_puts("Usage: sleep <seconds>\n");
        return;
    }
    
    int seconds = atoi(argv[1]);
    if (seconds <= 0) {
        vga_puts("Invalid number of seconds\n");
        return;
    }
    
    vga_printf("Sleeping for %d seconds...\n", seconds);
    sleep(seconds);
    vga_puts("Done!\n");
}

/* Built-in: demo - TUI demonstration */
void cmd_demo(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    
    /* Save screen state */
    vga_clear();
    
    /* Initialize TUI */
    tui_init();
    
    /* Draw title bar */
    tui_draw_titlebar(" NightOS - TUI Demo ");
    
    /* Draw main window */
    tui_window_t main_win;
    tui_window_create(&main_win, 5, 3, 70, 15, " System Information ", 
                      TUI_FLAG_BORDER | TUI_FLAG_SHADOW | TUI_FLAG_DOUBLE_BORDER);
    tui_window_draw(&main_win);
    
    /* Display system info in window */
    tui_window_print(&main_win, 2, 1, "NightOS v0.1.0 - Dark Theme Operating System");
    
    rtc_time_t time;
    char time_str[12];
    char date_str[12];
    rtc_read_time(&time);
    rtc_format_time(&time, time_str);
    rtc_format_date(&time, date_str);
    
    char info[64];
    strcpy(info, "Current Time: ");
    strcat(info, time_str);
    tui_window_print(&main_win, 2, 3, info);
    
    strcpy(info, "Current Date: ");
    strcat(info, date_str);
    tui_window_print(&main_win, 2, 4, info);
    
    uint32_t uptime = timer_get_seconds();
    tui_window_print(&main_win, 2, 6, "Uptime:");
    char uptime_str[20];
    itoa(uptime, uptime_str, 10);
    strcat(uptime_str, " seconds");
    tui_window_print(&main_win, 10, 6, uptime_str);
    
    memory_stats_t stats;
    memory_get_stats(&stats);
    tui_window_print(&main_win, 2, 8, "Memory:");
    char mem_str[32];
    itoa(stats.free_memory / 1024, mem_str, 10);
    strcat(mem_str, " KB free of ");
    char total_str[16];
    itoa(stats.total_memory / 1024, total_str, 10);
    strcat(mem_str, total_str);
    strcat(mem_str, " KB");
    tui_window_print(&main_win, 10, 8, mem_str);
    
    /* Draw progress bar */
    tui_window_print(&main_win, 2, 10, "Memory Usage:");
    int percent = (stats.used_memory * 100) / stats.total_memory;
    tui_draw_progress(main_win.x + 17, main_win.y + 11, 30, percent, VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    
    /* Draw buttons */
    tui_draw_button(main_win.x + 25, main_win.y + 13, " Press any key to exit ", true);
    
    /* Draw status bar */
    tui_draw_statusbar(" NightOS TUI Demo ", " Press any key... ");
    
    /* Wait for key */
    keyboard_getchar();
    
    /* Clear and return to shell */
    vga_clear();
    vga_set_color(vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    vga_puts("Returned to shell.\n");
}

/* Initialize shell */
void shell_init(void) {
    num_commands = 0;
    input_pos = 0;
    memset(input_buffer, 0, sizeof(input_buffer));
    
    /* Register built-in commands */
    shell_register_command("help", "Display available commands", cmd_help);
    shell_register_command("clear", "Clear the screen", cmd_clear);
    shell_register_command("echo", "Print text to screen", cmd_echo);
    shell_register_command("version", "Display OS version", cmd_version);
    shell_register_command("reboot", "Restart the system", cmd_reboot);
    shell_register_command("halt", "Halt the system", cmd_halt);
    shell_register_command("time", "Display system time/date", cmd_time);
    shell_register_command("uptime", "Show system uptime", cmd_uptime);
    shell_register_command("mem", "Display memory statistics", cmd_mem);
    shell_register_command("sleep", "Sleep for N seconds", cmd_sleep);
    shell_register_command("demo", "TUI demonstration", cmd_demo);
}

/* Main shell loop */
void shell_run(void) {
    shell_prompt();
    
    while (1) {
        char c = keyboard_getchar();
        
        if (c == '\n') {
            vga_putchar('\n');
            
            if (input_pos > 0) {
                input_buffer[input_pos] = '\0';
                shell_execute(input_buffer);
                input_pos = 0;
                memset(input_buffer, 0, sizeof(input_buffer));
            }
            
            shell_prompt();
        } else if (c == '\b') {
            if (input_pos > 0) {
                input_pos--;
                input_buffer[input_pos] = '\0';
                vga_backspace();
            }
        } else if (c >= ' ' && c <= '~') {
            if (input_pos < SHELL_MAX_INPUT - 1) {
                input_buffer[input_pos++] = c;
                vga_putchar(c);
            }
        }
    }
}
