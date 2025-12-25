/*
 * NightOS - Text User Interface (TUI)
 * 
 * Elegant dark-themed text UI framework
 */

#ifndef TUI_H
#define TUI_H

#include "types.h"
#include "vga.h"

/* Box drawing characters (using ASCII extended) */
#define BOX_HORIZONTAL  196  /* ─ */
#define BOX_VERTICAL    179  /* │ */
#define BOX_TOP_LEFT    218  /* ┌ */
#define BOX_TOP_RIGHT   191  /* ┐ */
#define BOX_BOTTOM_LEFT 192  /* └ */
#define BOX_BOTTOM_RIGHT 217 /* ┘ */
#define BOX_T_DOWN      194  /* ┬ */
#define BOX_T_UP        193  /* ┴ */
#define BOX_T_RIGHT     195  /* ├ */
#define BOX_T_LEFT      180  /* ┤ */
#define BOX_CROSS       197  /* ┼ */

/* Double-line box characters */
#define BOX_DOUBLE_H    205  /* ═ */
#define BOX_DOUBLE_V    186  /* ║ */
#define BOX_DOUBLE_TL   201  /* ╔ */
#define BOX_DOUBLE_TR   187  /* ╗ */
#define BOX_DOUBLE_BL   200  /* ╚ */
#define BOX_DOUBLE_BR   188  /* ╝ */

/* Block characters */
#define BLOCK_FULL      219  /* █ */
#define BLOCK_HALF      220  /* ▄ */
#define BLOCK_LIGHT     176  /* ░ */
#define BLOCK_MEDIUM    177  /* ▒ */
#define BLOCK_DARK      178  /* ▓ */

/* Theme structure */
typedef struct {
    uint8_t bg_color;
    uint8_t fg_color;
    uint8_t border_color;
    uint8_t title_bg;
    uint8_t title_fg;
    uint8_t highlight_bg;
    uint8_t highlight_fg;
    uint8_t shadow_color;
} tui_theme_t;

/* Window flags */
#define TUI_FLAG_BORDER       0x01
#define TUI_FLAG_SHADOW       0x02
#define TUI_FLAG_DOUBLE_BORDER 0x04
#define TUI_FLAG_TITLE        0x08

/* Window structure */
typedef struct {
    int x, y;
    int width, height;
    char title[64];
    uint8_t flags;
    bool visible;
} tui_window_t;

/* Menu structure */
#define TUI_MAX_MENU_ITEMS 16

typedef struct {
    char title[32];
    char items[TUI_MAX_MENU_ITEMS][32];
    int item_count;
    int selected;
    int x, y;
    bool visible;
} tui_menu_t;

/* Message box types */
#define TUI_MSG_INFO    0
#define TUI_MSG_WARNING 1
#define TUI_MSG_ERROR   2
#define TUI_MSG_SUCCESS 3

/* Initialize TUI */
void tui_init(void);

/* Theme functions */
void tui_set_theme(tui_theme_t* theme);
tui_theme_t* tui_get_theme(void);

/* Drawing primitives */
void tui_draw_hline(int x, int y, int width, uint8_t color);
void tui_draw_vline(int x, int y, int height, uint8_t color);
void tui_draw_box(int x, int y, int width, int height, uint8_t color, bool double_border);
void tui_fill_rect(int x, int y, int width, int height, uint8_t bg_color);
void tui_draw_text(int x, int y, const char* text, uint8_t fg, uint8_t bg);
void tui_draw_text_centered(int y, const char* text, uint8_t fg, uint8_t bg);

/* Window functions */
void tui_window_create(tui_window_t* win, int x, int y, int width, int height,
                       const char* title, uint8_t flags);
void tui_window_draw(tui_window_t* win);
void tui_window_clear(tui_window_t* win);
void tui_window_print(tui_window_t* win, int local_x, int local_y, const char* text);

/* Widget functions */
void tui_draw_button(int x, int y, const char* label, bool selected);
void tui_draw_progress(int x, int y, int width, int percent, uint8_t fg, uint8_t bg);

/* Menu functions */
void tui_menu_init(tui_menu_t* menu, const char* title, int x, int y);
void tui_menu_add_item(tui_menu_t* menu, const char* label);
void tui_menu_draw(tui_menu_t* menu);
int tui_menu_handle_input(tui_menu_t* menu, char key);

/* Message boxes */
void tui_message_box(const char* title, const char* message, uint8_t type);

/* Bars */
void tui_draw_statusbar(const char* left_text, const char* right_text);
void tui_draw_titlebar(const char* title);
void tui_draw_panel(int x, int y, int width, int height, const char* title);

#endif /* TUI_H */
