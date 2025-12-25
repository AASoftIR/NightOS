/*
 * NightOS - GUI Desktop Environment
 * 
 * Text-mode graphical desktop with windows and icons
 */

#ifndef GUI_H
#define GUI_H

#include "types.h"
#include "tui.h"

/* Desktop constants */
#define GUI_MAX_WINDOWS     8
#define GUI_MAX_ICONS       16
#define GUI_TASKBAR_HEIGHT  1
#define GUI_TITLEBAR_HEIGHT 1

/* Desktop icon structure */
typedef struct {
    char name[16];
    char icon;              /* ASCII character icon */
    uint8_t color;
    int x, y;
    void (*action)(void);   /* Function to call on activate */
    bool visible;
} gui_icon_t;

/* Desktop window structure */
typedef struct {
    tui_window_t base;
    int z_order;            /* Stacking order */
    bool focused;
    bool minimized;
    bool maximized;
    void (*on_draw)(void*); /* Custom draw callback */
    void (*on_key)(void*, char); /* Key handler */
    void* user_data;
} gui_window_t;

/* Clock widget */
typedef struct {
    int x, y;
    bool show_seconds;
    bool show_date;
} gui_clock_t;

/* Start menu entry */
typedef struct {
    char label[24];
    void (*action)(void);
} gui_menu_entry_t;

/* Desktop state */
typedef struct {
    bool running;
    gui_window_t* windows[GUI_MAX_WINDOWS];
    int window_count;
    gui_window_t* focused_window;
    gui_icon_t icons[GUI_MAX_ICONS];
    int icon_count;
    bool start_menu_open;
    gui_clock_t clock;
    uint8_t desktop_color;
    char wallpaper_char;
} gui_desktop_t;

/* Initialize GUI system */
void gui_init(void);

/* Desktop management */
void gui_run(void);
void gui_exit(void);
void gui_draw_desktop(void);
void gui_draw_taskbar(void);
void gui_draw_clock(void);
void gui_draw_start_menu(void);

/* Window management */
gui_window_t* gui_create_window(const char* title, int x, int y, int w, int h);
void gui_destroy_window(gui_window_t* win);
void gui_focus_window(gui_window_t* win);
void gui_minimize_window(gui_window_t* win);
void gui_maximize_window(gui_window_t* win);
void gui_restore_window(gui_window_t* win);
void gui_move_window(gui_window_t* win, int x, int y);
void gui_resize_window(gui_window_t* win, int w, int h);
void gui_bring_to_front(gui_window_t* win);
void gui_draw_window(gui_window_t* win);
void gui_draw_all_windows(void);

/* Icon management */
void gui_add_icon(const char* name, char icon, uint8_t color, int x, int y, void (*action)(void));
void gui_draw_icons(void);

/* Input handling */
void gui_handle_key(char key);

/* Built-in applications */
void gui_app_about(void);
void gui_app_sysinfo(void);
void gui_app_files(void);
void gui_app_settings(void);
void gui_app_terminal(void);

/* Get desktop instance */
gui_desktop_t* gui_get_desktop(void);

#endif /* GUI_H */
