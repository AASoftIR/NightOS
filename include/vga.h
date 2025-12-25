/*
 * NightOS - VGA Text Mode Driver
 * 
 * Provides text output capabilities with color support
 * Dark theme focused design
 */

#ifndef VGA_H
#define VGA_H

#include "types.h"

/* VGA text mode dimensions */
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

/* VGA memory address */
#define VGA_MEMORY 0xB8000

/* VGA colors */
typedef enum {
    VGA_COLOR_BLACK         = 0,
    VGA_COLOR_BLUE          = 1,
    VGA_COLOR_GREEN         = 2,
    VGA_COLOR_CYAN          = 3,
    VGA_COLOR_RED           = 4,
    VGA_COLOR_MAGENTA       = 5,
    VGA_COLOR_BROWN         = 6,
    VGA_COLOR_LIGHT_GREY    = 7,
    VGA_COLOR_DARK_GREY     = 8,
    VGA_COLOR_LIGHT_BLUE    = 9,
    VGA_COLOR_LIGHT_GREEN   = 10,
    VGA_COLOR_LIGHT_CYAN    = 11,
    VGA_COLOR_LIGHT_RED     = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_YELLOW        = 14,
    VGA_COLOR_WHITE         = 15,
} vga_color_t;

/* VGA entry structure */
typedef uint16_t vga_entry_t;

/* Create color attribute */
static inline uint8_t vga_color(vga_color_t fg, vga_color_t bg) {
    return fg | (bg << 4);
}

/* Create VGA entry */
static inline vga_entry_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

/* VGA driver functions */
void vga_init(void);
void vga_clear(void);
void vga_set_color(uint8_t color);
void vga_set_cursor(int x, int y);
void vga_get_cursor(int* x, int* y);
void vga_enable_cursor(uint8_t start, uint8_t end);
void vga_disable_cursor(void);
void vga_update_cursor(void);

/* Output functions */
void vga_putchar(char c);
void vga_puts(const char* str);
void vga_printf(const char* format, ...);
void vga_put_char_at(char c, int x, int y, uint8_t fg, uint8_t bg);

/* Color output */
void vga_putchar_color(char c, uint8_t color);
void vga_puts_color(const char* str, uint8_t color);

/* Scrolling */
void vga_scroll(void);

/* Position manipulation */
void vga_newline(void);
void vga_backspace(void);

#endif /* VGA_H */
