/*
 * NightOS - Text User Interface Implementation
 * 
 * Elegant TUI framework with windows, menus, and widgets
 */

#include "../include/tui.h"
#include "../include/vga.h"
#include "../include/string.h"
#include "../include/keyboard.h"

/* Global theme */
static tui_theme_t current_theme;

/* Initialize TUI with default dark theme */
void tui_init(void) {
    /* NightOS signature dark theme */
    current_theme.bg_color = VGA_COLOR_BLACK;
    current_theme.fg_color = VGA_COLOR_LIGHT_GREY;
    current_theme.border_color = VGA_COLOR_DARK_GREY;
    current_theme.title_bg = VGA_COLOR_DARK_GREY;
    current_theme.title_fg = VGA_COLOR_WHITE;
    current_theme.highlight_bg = VGA_COLOR_BLUE;
    current_theme.highlight_fg = VGA_COLOR_WHITE;
    current_theme.shadow_color = VGA_COLOR_BLACK;
    
    vga_clear();
}

/* Set custom theme */
void tui_set_theme(tui_theme_t* theme) {
    if (theme) {
        current_theme = *theme;
    }
}

/* Get current theme */
tui_theme_t* tui_get_theme(void) {
    return &current_theme;
}

/* Draw a single box character */
static void draw_char_at(int x, int y, char ch, uint8_t fg, uint8_t bg) {
    vga_put_char_at(ch, x, y, fg, bg);
}

/* Draw horizontal line */
void tui_draw_hline(int x, int y, int width, uint8_t color) {
    for (int i = 0; i < width; i++) {
        draw_char_at(x + i, y, BOX_HORIZONTAL, color, current_theme.bg_color);
    }
}

/* Draw vertical line */
void tui_draw_vline(int x, int y, int height, uint8_t color) {
    for (int i = 0; i < height; i++) {
        draw_char_at(x, y + i, BOX_VERTICAL, color, current_theme.bg_color);
    }
}

/* Draw a box/border */
void tui_draw_box(int x, int y, int width, int height, uint8_t color, bool double_border) {
    char h_char = double_border ? BOX_DOUBLE_H : BOX_HORIZONTAL;
    char v_char = double_border ? BOX_DOUBLE_V : BOX_VERTICAL;
    char tl_char = double_border ? BOX_DOUBLE_TL : BOX_TOP_LEFT;
    char tr_char = double_border ? BOX_DOUBLE_TR : BOX_TOP_RIGHT;
    char bl_char = double_border ? BOX_DOUBLE_BL : BOX_BOTTOM_LEFT;
    char br_char = double_border ? BOX_DOUBLE_BR : BOX_BOTTOM_RIGHT;
    
    /* Draw corners */
    draw_char_at(x, y, tl_char, color, current_theme.bg_color);
    draw_char_at(x + width - 1, y, tr_char, color, current_theme.bg_color);
    draw_char_at(x, y + height - 1, bl_char, color, current_theme.bg_color);
    draw_char_at(x + width - 1, y + height - 1, br_char, color, current_theme.bg_color);
    
    /* Draw horizontal lines */
    for (int i = 1; i < width - 1; i++) {
        draw_char_at(x + i, y, h_char, color, current_theme.bg_color);
        draw_char_at(x + i, y + height - 1, h_char, color, current_theme.bg_color);
    }
    
    /* Draw vertical lines */
    for (int i = 1; i < height - 1; i++) {
        draw_char_at(x, y + i, v_char, color, current_theme.bg_color);
        draw_char_at(x + width - 1, y + i, v_char, color, current_theme.bg_color);
    }
}

/* Fill a rectangular area */
void tui_fill_rect(int x, int y, int width, int height, uint8_t bg_color) {
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            draw_char_at(x + col, y + row, ' ', current_theme.fg_color, bg_color);
        }
    }
}

/* Draw text at position */
void tui_draw_text(int x, int y, const char* text, uint8_t fg, uint8_t bg) {
    int i = 0;
    while (text[i]) {
        draw_char_at(x + i, y, text[i], fg, bg);
        i++;
    }
}

/* Draw centered text */
void tui_draw_text_centered(int y, const char* text, uint8_t fg, uint8_t bg) {
    int len = strlen(text);
    int x = (VGA_WIDTH - len) / 2;
    tui_draw_text(x, y, text, fg, bg);
}

/* Create a window structure */
void tui_window_create(tui_window_t* win, int x, int y, int width, int height, 
                       const char* title, uint8_t flags) {
    win->x = x;
    win->y = y;
    win->width = width;
    win->height = height;
    win->flags = flags;
    win->visible = true;
    
    /* Copy title */
    int i = 0;
    while (title[i] && i < 63) {
        win->title[i] = title[i];
        i++;
    }
    win->title[i] = '\0';
}

/* Draw a window */
void tui_window_draw(tui_window_t* win) {
    if (!win->visible) return;
    
    bool has_shadow = (win->flags & TUI_FLAG_SHADOW) != 0;
    bool double_border = (win->flags & TUI_FLAG_DOUBLE_BORDER) != 0;
    
    /* Draw shadow first */
    if (has_shadow) {
        tui_fill_rect(win->x + 2, win->y + 1, win->width, win->height, 
                      current_theme.shadow_color);
    }
    
    /* Fill window background */
    tui_fill_rect(win->x, win->y, win->width, win->height, current_theme.bg_color);
    
    /* Draw border */
    tui_draw_box(win->x, win->y, win->width, win->height, 
                 current_theme.border_color, double_border);
    
    /* Draw title bar */
    if (win->title[0]) {
        /* Title background */
        for (int i = 1; i < win->width - 1; i++) {
            draw_char_at(win->x + i, win->y, ' ', 
                        current_theme.title_fg, current_theme.title_bg);
        }
        
        /* Title text (centered) */
        int title_len = strlen(win->title);
        int title_x = win->x + (win->width - title_len) / 2;
        tui_draw_text(title_x, win->y, win->title, 
                     current_theme.title_fg, current_theme.title_bg);
    }
}

/* Clear window content area */
void tui_window_clear(tui_window_t* win) {
    tui_fill_rect(win->x + 1, win->y + 1, win->width - 2, win->height - 2, 
                  current_theme.bg_color);
}

/* Draw text inside window */
void tui_window_print(tui_window_t* win, int local_x, int local_y, const char* text) {
    tui_draw_text(win->x + 1 + local_x, win->y + 1 + local_y, text, 
                  current_theme.fg_color, current_theme.bg_color);
}

/* Draw a button */
void tui_draw_button(int x, int y, const char* label, bool selected) {
    uint8_t fg = selected ? current_theme.highlight_fg : current_theme.fg_color;
    uint8_t bg = selected ? current_theme.highlight_bg : current_theme.bg_color;
    
    int len = strlen(label);
    
    /* Draw button brackets and label */
    draw_char_at(x, y, '[', current_theme.border_color, current_theme.bg_color);
    
    for (int i = 0; i < len; i++) {
        draw_char_at(x + 1 + i, y, label[i], fg, bg);
    }
    
    draw_char_at(x + len + 1, y, ']', current_theme.border_color, current_theme.bg_color);
}

/* Draw progress bar */
void tui_draw_progress(int x, int y, int width, int percent, uint8_t fg, uint8_t bg) {
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    
    int filled = (width * percent) / 100;
    
    draw_char_at(x, y, '[', current_theme.border_color, current_theme.bg_color);
    
    for (int i = 0; i < width; i++) {
        if (i < filled) {
            draw_char_at(x + 1 + i, y, BLOCK_FULL, fg, bg);
        } else {
            draw_char_at(x + 1 + i, y, BLOCK_LIGHT, current_theme.border_color, bg);
        }
    }
    
    draw_char_at(x + width + 1, y, ']', current_theme.border_color, current_theme.bg_color);
}

/* Initialize a menu */
void tui_menu_init(tui_menu_t* menu, const char* title, int x, int y) {
    menu->x = x;
    menu->y = y;
    menu->item_count = 0;
    menu->selected = 0;
    menu->visible = false;
    
    int i = 0;
    while (title[i] && i < 31) {
        menu->title[i] = title[i];
        i++;
    }
    menu->title[i] = '\0';
}

/* Add item to menu */
void tui_menu_add_item(tui_menu_t* menu, const char* label) {
    if (menu->item_count >= TUI_MAX_MENU_ITEMS) return;
    
    int i = 0;
    while (label[i] && i < 31) {
        menu->items[menu->item_count][i] = label[i];
        i++;
    }
    menu->items[menu->item_count][i] = '\0';
    menu->item_count++;
}

/* Draw menu */
void tui_menu_draw(tui_menu_t* menu) {
    if (!menu->visible) return;
    
    /* Calculate menu dimensions */
    int max_width = strlen(menu->title);
    for (int i = 0; i < menu->item_count; i++) {
        int len = strlen(menu->items[i]);
        if (len > max_width) max_width = len;
    }
    
    int width = max_width + 4;
    int height = menu->item_count + 2;
    
    /* Draw menu window */
    tui_fill_rect(menu->x, menu->y, width, height, current_theme.bg_color);
    tui_draw_box(menu->x, menu->y, width, height, current_theme.border_color, false);
    
    /* Draw title */
    tui_draw_text(menu->x + 2, menu->y, menu->title, 
                  current_theme.title_fg, current_theme.title_bg);
    
    /* Draw items */
    for (int i = 0; i < menu->item_count; i++) {
        uint8_t fg = (i == menu->selected) ? current_theme.highlight_fg : current_theme.fg_color;
        uint8_t bg = (i == menu->selected) ? current_theme.highlight_bg : current_theme.bg_color;
        
        /* Fill item background */
        for (int j = 1; j < width - 1; j++) {
            draw_char_at(menu->x + j, menu->y + 1 + i, ' ', fg, bg);
        }
        
        tui_draw_text(menu->x + 2, menu->y + 1 + i, menu->items[i], fg, bg);
    }
}

/* Handle menu input - returns selected index or -1 */
int tui_menu_handle_input(tui_menu_t* menu, char key) {
    if (!menu->visible) return -1;
    
    if (key == 'w' || key == 'W') {  /* Up */
        if (menu->selected > 0) {
            menu->selected--;
        }
    } else if (key == 's' || key == 'S') {  /* Down */
        if (menu->selected < menu->item_count - 1) {
            menu->selected++;
        }
    } else if (key == '\n') {  /* Enter */
        return menu->selected;
    } else if (key == 27) {  /* Escape */
        menu->visible = false;
        return -2;  /* Cancelled */
    }
    
    return -1;  /* No selection yet */
}

/* Show a message box - blocks until dismissed */
void tui_message_box(const char* title, const char* message, uint8_t type) {
    int msg_len = strlen(message);
    int title_len = strlen(title);
    int width = (msg_len > title_len ? msg_len : title_len) + 6;
    if (width < 20) width = 20;
    
    int height = 5;
    int x = (VGA_WIDTH - width) / 2;
    int y = (VGA_HEIGHT - height) / 2;
    
    /* Choose icon based on type */
    char icon = ' ';
    uint8_t icon_color = current_theme.fg_color;
    switch (type) {
        case TUI_MSG_INFO:
            icon = 'i';
            icon_color = VGA_COLOR_CYAN;
            break;
        case TUI_MSG_WARNING:
            icon = '!';
            icon_color = VGA_COLOR_YELLOW;
            break;
        case TUI_MSG_ERROR:
            icon = 'X';
            icon_color = VGA_COLOR_LIGHT_RED;
            break;
        case TUI_MSG_SUCCESS:
            icon = '*';
            icon_color = VGA_COLOR_GREEN;
            break;
    }
    
    /* Draw message box */
    tui_fill_rect(x + 2, y + 1, width, height, current_theme.shadow_color);  /* Shadow */
    tui_fill_rect(x, y, width, height, current_theme.bg_color);
    tui_draw_box(x, y, width, height, current_theme.border_color, true);
    
    /* Title */
    int title_x = x + (width - title_len) / 2;
    tui_draw_text(title_x, y, title, current_theme.title_fg, current_theme.title_bg);
    
    /* Icon and message */
    draw_char_at(x + 2, y + 2, icon, icon_color, current_theme.bg_color);
    tui_draw_text(x + 4, y + 2, message, current_theme.fg_color, current_theme.bg_color);
    
    /* OK button */
    tui_draw_button(x + width/2 - 3, y + height - 2, " OK ", true);
}

/* Draw status bar at bottom of screen */
void tui_draw_statusbar(const char* left_text, const char* right_text) {
    int y = VGA_HEIGHT - 1;
    
    /* Fill status bar */
    for (int i = 0; i < VGA_WIDTH; i++) {
        draw_char_at(i, y, ' ', current_theme.title_fg, current_theme.title_bg);
    }
    
    /* Left text */
    if (left_text) {
        tui_draw_text(1, y, left_text, current_theme.title_fg, current_theme.title_bg);
    }
    
    /* Right text */
    if (right_text) {
        int len = strlen(right_text);
        tui_draw_text(VGA_WIDTH - len - 1, y, right_text, 
                      current_theme.title_fg, current_theme.title_bg);
    }
}

/* Draw title bar at top of screen */
void tui_draw_titlebar(const char* title) {
    /* Fill title bar */
    for (int i = 0; i < VGA_WIDTH; i++) {
        draw_char_at(i, 0, ' ', current_theme.title_fg, current_theme.title_bg);
    }
    
    /* Center title */
    tui_draw_text_centered(0, title, current_theme.title_fg, current_theme.title_bg);
}

/* Draw a simple frame/panel */
void tui_draw_panel(int x, int y, int width, int height, const char* title) {
    tui_fill_rect(x, y, width, height, current_theme.bg_color);
    tui_draw_box(x, y, width, height, current_theme.border_color, false);
    
    if (title && title[0]) {
        int title_len = strlen(title);
        int title_x = x + (width - title_len - 2) / 2;
        
        draw_char_at(title_x, y, ' ', current_theme.fg_color, current_theme.bg_color);
        tui_draw_text(title_x + 1, y, title, current_theme.title_fg, current_theme.bg_color);
        draw_char_at(title_x + title_len + 1, y, ' ', current_theme.fg_color, current_theme.bg_color);
    }
}
