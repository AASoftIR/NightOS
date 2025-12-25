/*
 * NightOS - VGA Text Mode Driver Implementation
 * 
 * Dark theme focused text output
 */

#include "../include/vga.h"
#include "../include/io.h"
#include "../include/string.h"

/* VGA I/O ports */
#define VGA_CTRL_PORT 0x3D4
#define VGA_DATA_PORT 0x3D5

/* Global VGA state */
static uint16_t* vga_buffer;
static int vga_row;
static int vga_col;
static uint8_t vga_current_color;

/* Initialize VGA driver */
void vga_init(void) {
    vga_buffer = (uint16_t*)VGA_MEMORY;
    vga_row = 0;
    vga_col = 0;
    /* Dark theme: light grey on black */
    vga_current_color = vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    vga_clear();
    vga_enable_cursor(14, 15);
}

/* Clear the screen */
void vga_clear(void) {
    vga_entry_t blank = vga_entry(' ', vga_current_color);
    
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = blank;
    }
    
    vga_row = 0;
    vga_col = 0;
    vga_update_cursor();
}

/* Set current color */
void vga_set_color(uint8_t color) {
    vga_current_color = color;
}

/* Set cursor position */
void vga_set_cursor(int x, int y) {
    vga_col = x;
    vga_row = y;
    vga_update_cursor();
}

/* Get cursor position */
void vga_get_cursor(int* x, int* y) {
    *x = vga_col;
    *y = vga_row;
}

/* Enable cursor */
void vga_enable_cursor(uint8_t start, uint8_t end) {
    outb(VGA_CTRL_PORT, 0x0A);
    outb(VGA_DATA_PORT, (inb(VGA_DATA_PORT) & 0xC0) | start);
    
    outb(VGA_CTRL_PORT, 0x0B);
    outb(VGA_DATA_PORT, (inb(VGA_DATA_PORT) & 0xE0) | end);
}

/* Disable cursor */
void vga_disable_cursor(void) {
    outb(VGA_CTRL_PORT, 0x0A);
    outb(VGA_DATA_PORT, 0x20);
}

/* Update hardware cursor position */
void vga_update_cursor(void) {
    uint16_t pos = vga_row * VGA_WIDTH + vga_col;
    
    outb(VGA_CTRL_PORT, 0x0F);
    outb(VGA_DATA_PORT, (uint8_t)(pos & 0xFF));
    outb(VGA_CTRL_PORT, 0x0E);
    outb(VGA_DATA_PORT, (uint8_t)((pos >> 8) & 0xFF));
}

/* Scroll screen up by one line */
void vga_scroll(void) {
    vga_entry_t blank = vga_entry(' ', vga_current_color);
    
    /* Move all lines up */
    for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
        vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
    }
    
    /* Clear last line */
    for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
        vga_buffer[i] = blank;
    }
    
    vga_row = VGA_HEIGHT - 1;
}

/* Handle newline */
void vga_newline(void) {
    vga_col = 0;
    vga_row++;
    
    if (vga_row >= VGA_HEIGHT) {
        vga_scroll();
    }
    
    vga_update_cursor();
}

/* Handle backspace */
void vga_backspace(void) {
    if (vga_col > 0) {
        vga_col--;
    } else if (vga_row > 0) {
        vga_row--;
        vga_col = VGA_WIDTH - 1;
    }
    
    int index = vga_row * VGA_WIDTH + vga_col;
    vga_buffer[index] = vga_entry(' ', vga_current_color);
    vga_update_cursor();
}

/* Put a single character with current color */
void vga_putchar(char c) {
    vga_putchar_color(c, vga_current_color);
}

/* Put a single character with specified color */
void vga_putchar_color(char c, uint8_t color) {
    switch (c) {
        case '\n':
            vga_newline();
            return;
        case '\r':
            vga_col = 0;
            vga_update_cursor();
            return;
        case '\t':
            vga_col = (vga_col + 8) & ~7;
            if (vga_col >= VGA_WIDTH) {
                vga_newline();
            }
            vga_update_cursor();
            return;
        case '\b':
            vga_backspace();
            return;
    }
    
    int index = vga_row * VGA_WIDTH + vga_col;
    vga_buffer[index] = vga_entry(c, color);
    
    vga_col++;
    if (vga_col >= VGA_WIDTH) {
        vga_newline();
    } else {
        vga_update_cursor();
    }
}

/* Put a string with current color */
void vga_puts(const char* str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

/* Put a string with specified color */
void vga_puts_color(const char* str, uint8_t color) {
    while (*str) {
        vga_putchar_color(*str++, color);
    }
}

/* Simple printf implementation */
void vga_printf(const char* format, ...) {
    char buffer[32];
    __builtin_va_list args;
    __builtin_va_start(args, format);
    
    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd':
                case 'i': {
                    int val = __builtin_va_arg(args, int);
                    itoa(val, buffer, 10);
                    vga_puts(buffer);
                    break;
                }
                case 'u': {
                    uint32_t val = __builtin_va_arg(args, uint32_t);
                    utoa(val, buffer, 10);
                    vga_puts(buffer);
                    break;
                }
                case 'x': {
                    uint32_t val = __builtin_va_arg(args, uint32_t);
                    utoa(val, buffer, 16);
                    vga_puts(buffer);
                    break;
                }
                case 's': {
                    char* str = __builtin_va_arg(args, char*);
                    vga_puts(str ? str : "(null)");
                    break;
                }
                case 'c': {
                    char c = (char)__builtin_va_arg(args, int);
                    vga_putchar(c);
                    break;
                }
                case '%':
                    vga_putchar('%');
                    break;
                default:
                    vga_putchar('%');
                    vga_putchar(*format);
                    break;
            }
        } else {
            vga_putchar(*format);
        }
        format++;
    }
    
    __builtin_va_end(args);
}
