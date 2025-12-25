/*
 * NightOS Kernel
 * 
 * Main kernel entry point and initialization
 * A minimal, dark-themed operating system
 */

#include "../include/types.h"
#include "../include/config.h"
#include "../include/vga.h"
#include "../include/keyboard.h"
#include "../include/shell.h"
#include "../include/string.h"
#include "../include/idt.h"
#include "../include/timer.h"
#include "../include/rtc.h"
#include "../include/memory.h"
#include "../include/tui.h"
#include "../include/fs.h"
#include "../include/process.h"
#include "../include/syscall.h"
#include "../include/gui.h"

/* Forward declarations */
static void display_boot_logo(void);
static void kernel_init(void);

/*
 * Kernel main entry point
 * Called by the bootloader after switching to protected mode
 */
void kernel_main(void) {
    kernel_init();
    
    display_boot_logo();
    
    /* Enter shell */
    vga_set_color(vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    vga_puts("Type 'help' for available commands.\n\n");
    
    shell_run();
    
    /* Should never reach here */
    while (1) {
        __asm__ volatile("hlt");
    }
}

/*
 * Initialize kernel subsystems
 */
static void kernel_init(void) {
    /* Initialize VGA driver */
    vga_init();
    
    /* Initialize IDT (interrupts) */
    idt_init();
    
    /* Initialize PIT timer (100 Hz) */
    timer_init(TIMER_FREQUENCY);
    
    /* Initialize RTC */
    rtc_init();
    
    /* Initialize memory manager */
    memory_init();
    
    /* Initialize filesystem */
    fs_init();
    
    /* Initialize process manager */
    process_init();
    
    /* Initialize system calls */
    syscall_init();
    
    /* Initialize GUI */
    gui_init();
    
    /* Enable interrupts */
    __asm__ volatile("sti");
    
    /* Initialize keyboard driver */
    keyboard_init();
    
    /* Initialize shell */
    shell_init();
}

/*
 * Display boot logo
 */
static void display_boot_logo(void) {
    /* Dark blue header bar */
    vga_set_color(vga_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE));
    for (int i = 0; i < VGA_WIDTH; i++) {
        vga_putchar(' ');
    }
    vga_set_cursor(0, 0);
    vga_puts("  NightOS v0.1.0 - Minimal Dark Theme Operating System");
    
    /* Move to next line and reset color */
    vga_set_cursor(0, 2);
    vga_set_color(vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    
    /* ASCII art logo */
    vga_set_color(vga_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK));
    vga_puts("    _   ___       __    __  ____  _____\n");
    vga_puts("   / | / (_)___ _/ /_  / /_/ __ \\/ ___/\n");
    vga_puts("  /  |/ / / __ `/ __ \\/ __/ / / /\\__ \\ \n");
    vga_puts(" / /|  / / /_/ / / / / /_/ /_/ /___/ / \n");
    vga_puts("/_/ |_/_/\\__, /_/ /_/\\__/\\____//____/  \n");
    vga_puts("        /____/                         \n");
    
    vga_set_color(vga_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK));
    vga_puts("\n  A minimal operating system inspired by Unix/Linux\n");
    vga_puts("  Dark theme | Elegant | Minimal\n");
    
    vga_set_color(vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    vga_puts("\n");
    vga_puts("  ------------------------------------------------\n");
    vga_puts("\n");
}
