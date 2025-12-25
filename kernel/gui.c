/*
 * NightOS - GUI Desktop Environment Implementation
 * 
 * Text-mode graphical desktop with windows, icons, and taskbar
 */

#include "../include/gui.h"
#include "../include/vga.h"
#include "../include/keyboard.h"
#include "../include/string.h"
#include "../include/memory.h"
#include "../include/timer.h"
#include "../include/rtc.h"
#include "../include/process.h"
#include "../include/fs.h"

/* Global desktop instance */
static gui_desktop_t desktop;

/* Start menu entries */
#define START_MENU_ITEMS 6
static gui_menu_entry_t start_menu[START_MENU_ITEMS] = {
    {"Terminal",  gui_app_terminal},
    {"Files",     gui_app_files},
    {"System",    gui_app_sysinfo},
    {"Settings",  gui_app_settings},
    {"About",     gui_app_about},
    {"Exit",      gui_exit}
};

/* Initialize GUI */
void gui_init(void) {
    memset(&desktop, 0, sizeof(desktop));
    
    desktop.running = false;
    desktop.window_count = 0;
    desktop.focused_window = NULL;
    desktop.icon_count = 0;
    desktop.start_menu_open = false;
    desktop.desktop_color = VGA_COLOR_BLUE;
    desktop.wallpaper_char = BLOCK_LIGHT;
    
    /* Clock settings */
    desktop.clock.x = VGA_WIDTH - 10;
    desktop.clock.y = VGA_HEIGHT - 1;
    desktop.clock.show_seconds = true;
    desktop.clock.show_date = false;
    
    /* Add default icons */
    gui_add_icon("Term", '$', VGA_COLOR_WHITE, 3, 3, gui_app_terminal);
    gui_add_icon("Files", '*', VGA_COLOR_YELLOW, 3, 6, gui_app_files);
    gui_add_icon("Info", 'i', VGA_COLOR_CYAN, 3, 9, gui_app_sysinfo);
}

/* Get desktop instance */
gui_desktop_t* gui_get_desktop(void) {
    return &desktop;
}

/* Draw desktop background */
void gui_draw_desktop(void) {
    /* Fill with wallpaper pattern */
    for (int y = 1; y < VGA_HEIGHT - 1; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_put_char_at(desktop.wallpaper_char, x, y, 
                           VGA_COLOR_DARK_GREY, desktop.desktop_color);
        }
    }
}

/* Draw taskbar at bottom */
void gui_draw_taskbar(void) {
    int y = VGA_HEIGHT - 1;
    
    /* Fill taskbar */
    for (int x = 0; x < VGA_WIDTH; x++) {
        vga_put_char_at(' ', x, y, VGA_COLOR_WHITE, VGA_COLOR_DARK_GREY);
    }
    
    /* Start button */
    vga_put_char_at('[', 0, y, VGA_COLOR_WHITE, VGA_COLOR_DARK_GREY);
    vga_put_char_at('S', 1, y, VGA_COLOR_WHITE, VGA_COLOR_GREEN);
    vga_put_char_at('t', 2, y, VGA_COLOR_WHITE, VGA_COLOR_GREEN);
    vga_put_char_at('a', 3, y, VGA_COLOR_WHITE, VGA_COLOR_GREEN);
    vga_put_char_at('r', 4, y, VGA_COLOR_WHITE, VGA_COLOR_GREEN);
    vga_put_char_at('t', 5, y, VGA_COLOR_WHITE, VGA_COLOR_GREEN);
    vga_put_char_at(']', 6, y, VGA_COLOR_WHITE, VGA_COLOR_DARK_GREY);
    
    /* Draw open windows in taskbar */
    int tx = 8;
    for (int i = 0; i < desktop.window_count && tx < VGA_WIDTH - 15; i++) {
        gui_window_t* win = desktop.windows[i];
        if (win && !win->minimized) {
            uint8_t bg = (win == desktop.focused_window) ? VGA_COLOR_BLUE : VGA_COLOR_DARK_GREY;
            
            vga_put_char_at('[', tx++, y, VGA_COLOR_WHITE, VGA_COLOR_DARK_GREY);
            
            int len = strlen(win->base.title);
            if (len > 8) len = 8;
            for (int j = 0; j < len; j++) {
                vga_put_char_at(win->base.title[j], tx++, y, VGA_COLOR_WHITE, bg);
            }
            
            vga_put_char_at(']', tx++, y, VGA_COLOR_WHITE, VGA_COLOR_DARK_GREY);
            tx++;
        }
    }
}

/* Draw clock */
void gui_draw_clock(void) {
    rtc_time_t time;
    rtc_read_time(&time);
    
    char str[12];
    int y = VGA_HEIGHT - 1;
    int x = VGA_WIDTH - 9;
    
    /* Format time */
    str[0] = '0' + (time.hour / 10);
    str[1] = '0' + (time.hour % 10);
    str[2] = ':';
    str[3] = '0' + (time.minute / 10);
    str[4] = '0' + (time.minute % 10);
    str[5] = ':';
    str[6] = '0' + (time.second / 10);
    str[7] = '0' + (time.second % 10);
    str[8] = '\0';
    
    for (int i = 0; str[i]; i++) {
        vga_put_char_at(str[i], x + i, y, VGA_COLOR_WHITE, VGA_COLOR_DARK_GREY);
    }
}

/* Draw start menu */
void gui_draw_start_menu(void) {
    if (!desktop.start_menu_open) return;
    
    int x = 0;
    int y = VGA_HEIGHT - 2 - START_MENU_ITEMS;
    int width = 14;
    int height = START_MENU_ITEMS + 2;
    
    /* Draw menu background */
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            vga_put_char_at(' ', x + col, y + row, 
                           VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        }
    }
    
    /* Draw border */
    for (int col = 0; col < width; col++) {
        vga_put_char_at(BOX_HORIZONTAL, x + col, y, 
                       VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    }
    vga_put_char_at(BOX_TOP_LEFT, x, y, VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    vga_put_char_at(BOX_TOP_RIGHT, x + width - 1, y, VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    
    for (int row = 1; row < height - 1; row++) {
        vga_put_char_at(BOX_VERTICAL, x, y + row, VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
        vga_put_char_at(BOX_VERTICAL, x + width - 1, y + row, VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    }
    
    /* Draw menu items */
    for (int i = 0; i < START_MENU_ITEMS; i++) {
        for (int j = 0; j < (int)strlen(start_menu[i].label) && j < width - 3; j++) {
            vga_put_char_at(start_menu[i].label[j], x + 2 + j, y + 1 + i,
                           VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        }
    }
}

/* Draw desktop icons */
void gui_draw_icons(void) {
    for (int i = 0; i < desktop.icon_count; i++) {
        gui_icon_t* icon = &desktop.icons[i];
        if (!icon->visible) continue;
        
        /* Draw icon character */
        vga_put_char_at(icon->icon, icon->x, icon->y, 
                       icon->color, desktop.desktop_color);
        
        /* Draw icon name below */
        int len = strlen(icon->name);
        for (int j = 0; j < len; j++) {
            vga_put_char_at(icon->name[j], icon->x - len/2 + j, icon->y + 1,
                           VGA_COLOR_WHITE, desktop.desktop_color);
        }
    }
}

/* Add desktop icon */
void gui_add_icon(const char* name, char icon, uint8_t color, int x, int y, void (*action)(void)) {
    if (desktop.icon_count >= GUI_MAX_ICONS) return;
    
    gui_icon_t* ic = &desktop.icons[desktop.icon_count++];
    strncpy(ic->name, name, 15);
    ic->name[15] = '\0';
    ic->icon = icon;
    ic->color = color;
    ic->x = x;
    ic->y = y;
    ic->action = action;
    ic->visible = true;
}

/* Create a window */
gui_window_t* gui_create_window(const char* title, int x, int y, int w, int h) {
    if (desktop.window_count >= GUI_MAX_WINDOWS) return NULL;
    
    gui_window_t* win = (gui_window_t*)kmalloc(sizeof(gui_window_t));
    if (!win) return NULL;
    
    memset(win, 0, sizeof(gui_window_t));
    
    tui_window_create(&win->base, x, y, w, h, title, 
                      TUI_FLAG_BORDER | TUI_FLAG_SHADOW);
    
    win->z_order = desktop.window_count;
    win->focused = true;
    win->minimized = false;
    win->maximized = false;
    
    /* Unfocus other windows */
    for (int i = 0; i < desktop.window_count; i++) {
        if (desktop.windows[i]) {
            desktop.windows[i]->focused = false;
        }
    }
    
    desktop.windows[desktop.window_count++] = win;
    desktop.focused_window = win;
    
    return win;
}

/* Destroy a window */
void gui_destroy_window(gui_window_t* win) {
    if (!win) return;
    
    /* Remove from list */
    for (int i = 0; i < desktop.window_count; i++) {
        if (desktop.windows[i] == win) {
            for (int j = i; j < desktop.window_count - 1; j++) {
                desktop.windows[j] = desktop.windows[j + 1];
            }
            desktop.window_count--;
            break;
        }
    }
    
    /* Update focus */
    if (desktop.focused_window == win) {
        desktop.focused_window = desktop.window_count > 0 ? 
                                 desktop.windows[desktop.window_count - 1] : NULL;
        if (desktop.focused_window) {
            desktop.focused_window->focused = true;
        }
    }
    
    kfree(win);
}

/* Focus a window */
void gui_focus_window(gui_window_t* win) {
    if (!win) return;
    
    for (int i = 0; i < desktop.window_count; i++) {
        if (desktop.windows[i]) {
            desktop.windows[i]->focused = (desktop.windows[i] == win);
        }
    }
    desktop.focused_window = win;
}

/* Draw a single window */
void gui_draw_window(gui_window_t* win) {
    if (!win || win->minimized) return;
    
    /* Draw window frame */
    tui_window_draw(&win->base);
    
    /* Draw close button */
    int bx = win->base.x + win->base.width - 4;
    int by = win->base.y;
    vga_put_char_at('[', bx, by, VGA_COLOR_DARK_GREY, VGA_COLOR_DARK_GREY);
    vga_put_char_at('X', bx + 1, by, VGA_COLOR_RED, VGA_COLOR_DARK_GREY);
    vga_put_char_at(']', bx + 2, by, VGA_COLOR_DARK_GREY, VGA_COLOR_DARK_GREY);
    
    /* Call custom draw callback */
    if (win->on_draw) {
        win->on_draw(win);
    }
}

/* Draw all windows */
void gui_draw_all_windows(void) {
    for (int i = 0; i < desktop.window_count; i++) {
        gui_draw_window(desktop.windows[i]);
    }
}

/* Handle keyboard input */
void gui_handle_key(char key) {
    /* Tab to toggle start menu */
    if (key == '\t') {
        desktop.start_menu_open = !desktop.start_menu_open;
        return;
    }
    
    /* Escape to close menu or focused window */
    if (key == 27) {
        if (desktop.start_menu_open) {
            desktop.start_menu_open = false;
        } else if (desktop.focused_window) {
            gui_destroy_window(desktop.focused_window);
        }
        return;
    }
    
    /* Number keys 1-6 for start menu */
    if (desktop.start_menu_open && key >= '1' && key <= '6') {
        int idx = key - '1';
        if (idx < START_MENU_ITEMS && start_menu[idx].action) {
            desktop.start_menu_open = false;
            start_menu[idx].action();
        }
        return;
    }
    
    /* Pass to focused window */
    if (desktop.focused_window && desktop.focused_window->on_key) {
        desktop.focused_window->on_key(desktop.focused_window, key);
    }
}

/* Main GUI loop */
void gui_run(void) {
    desktop.running = true;
    
    while (desktop.running) {
        /* Draw everything */
        gui_draw_desktop();
        gui_draw_icons();
        gui_draw_all_windows();
        gui_draw_taskbar();
        gui_draw_clock();
        gui_draw_start_menu();
        
        /* Check for input */
        char key = keyboard_getchar();
        gui_handle_key(key);
    }
    
    /* Cleanup */
    while (desktop.window_count > 0) {
        gui_destroy_window(desktop.windows[0]);
    }
    
    vga_clear();
}

/* Exit desktop */
void gui_exit(void) {
    desktop.running = false;
}

/* ========== Built-in Applications ========== */

/* About dialog draw callback */
static void about_draw(void* data) {
    gui_window_t* win = (gui_window_t*)data;
    
    tui_draw_text(win->base.x + 3, win->base.y + 2, "NightOS v0.1.0", 
                  VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    tui_draw_text(win->base.x + 3, win->base.y + 4, "A minimal dark-themed OS",
                  VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    tui_draw_text(win->base.x + 3, win->base.y + 5, "Written in C & Assembly",
                  VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    tui_draw_text(win->base.x + 3, win->base.y + 7, "Press ESC to close",
                  VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
}

void gui_app_about(void) {
    gui_window_t* win = gui_create_window("About NightOS", 20, 6, 40, 12);
    if (win) {
        win->on_draw = about_draw;
    }
}

/* System info draw callback */
static void sysinfo_draw(void* data) {
    gui_window_t* win = (gui_window_t*)data;
    char buf[40];
    
    /* Uptime */
    uint32_t uptime = timer_get_seconds();
    tui_draw_text(win->base.x + 2, win->base.y + 2, "Uptime:", 
                  VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    itoa(uptime, buf, 10);
    strcat(buf, " seconds");
    tui_draw_text(win->base.x + 12, win->base.y + 2, buf,
                  VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    
    /* Memory */
    memory_stats_t stats;
    memory_get_stats(&stats);
    
    tui_draw_text(win->base.x + 2, win->base.y + 4, "Memory:", 
                  VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    itoa(stats.free_memory / 1024, buf, 10);
    strcat(buf, " KB free");
    tui_draw_text(win->base.x + 12, win->base.y + 4, buf,
                  VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    
    /* Processes */
    tui_draw_text(win->base.x + 2, win->base.y + 6, "Processes:", 
                  VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    itoa(process_count(), buf, 10);
    tui_draw_text(win->base.x + 12, win->base.y + 6, buf,
                  VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    
    /* Files */
    tui_draw_text(win->base.x + 2, win->base.y + 8, "Files:", 
                  VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    itoa(fs_count(), buf, 10);
    tui_draw_text(win->base.x + 12, win->base.y + 8, buf,
                  VGA_COLOR_CYAN, VGA_COLOR_BLACK);
}

void gui_app_sysinfo(void) {
    gui_window_t* win = gui_create_window("System Info", 15, 5, 45, 14);
    if (win) {
        win->on_draw = sysinfo_draw;
    }
}

/* File manager draw callback */
static void files_draw(void* data) {
    gui_window_t* win = (gui_window_t*)data;
    
    fs_dirent_t entries[16];
    int count = fs_list(entries, 16);
    
    tui_draw_text(win->base.x + 2, win->base.y + 2, "Name          Size", 
                  VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    tui_draw_text(win->base.x + 2, win->base.y + 3, "-------------------", 
                  VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    
    for (int i = 0; i < count && i < 10; i++) {
        char line[32];
        char size_str[10];
        
        strncpy(line, entries[i].name, 12);
        line[12] = '\0';
        
        /* Pad name */
        int len = strlen(line);
        while (len < 14) line[len++] = ' ';
        line[14] = '\0';
        
        itoa(entries[i].size, size_str, 10);
        strcat(line, size_str);
        
        uint8_t color = (entries[i].type == FS_TYPE_DIRECTORY) ? 
                        VGA_COLOR_CYAN : VGA_COLOR_LIGHT_GREY;
        tui_draw_text(win->base.x + 2, win->base.y + 4 + i, line,
                      color, VGA_COLOR_BLACK);
    }
}

void gui_app_files(void) {
    gui_window_t* win = gui_create_window("File Manager", 10, 4, 50, 18);
    if (win) {
        win->on_draw = files_draw;
    }
}

/* Settings draw callback */
static void settings_draw(void* data) {
    gui_window_t* win = (gui_window_t*)data;
    
    tui_draw_text(win->base.x + 2, win->base.y + 2, "Desktop Color:", 
                  VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    tui_draw_text(win->base.x + 18, win->base.y + 2, "[B]lue [G]reen [R]ed",
                  VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    tui_draw_text(win->base.x + 2, win->base.y + 4, "Clock Seconds:", 
                  VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    tui_draw_text(win->base.x + 18, win->base.y + 4, 
                  desktop.clock.show_seconds ? "[ON] off" : "on [OFF]",
                  VGA_COLOR_CYAN, VGA_COLOR_BLACK);
}

static void settings_key(void* data, char key) {
    UNUSED(data);
    
    switch (key) {
        case 'b': case 'B':
            desktop.desktop_color = VGA_COLOR_BLUE;
            break;
        case 'g': case 'G':
            desktop.desktop_color = VGA_COLOR_GREEN;
            break;
        case 'r': case 'R':
            desktop.desktop_color = VGA_COLOR_RED;
            break;
        case 's': case 'S':
            desktop.clock.show_seconds = !desktop.clock.show_seconds;
            break;
    }
}

void gui_app_settings(void) {
    gui_window_t* win = gui_create_window("Settings", 18, 7, 44, 10);
    if (win) {
        win->on_draw = settings_draw;
        win->on_key = settings_key;
    }
}

/* Terminal - exits GUI and returns to shell */
void gui_app_terminal(void) {
    gui_exit();
}
