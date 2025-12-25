/*
 * NightOS - Keyboard Driver Header
 * 
 * PS/2 keyboard driver with scan code translation
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

/* Keyboard I/O ports */
#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

/* Special keys */
#define KEY_ESCAPE      0x01
#define KEY_BACKSPACE   0x0E
#define KEY_TAB         0x0F
#define KEY_ENTER       0x1C
#define KEY_LCTRL       0x1D
#define KEY_LSHIFT      0x2A
#define KEY_RSHIFT      0x36
#define KEY_LALT        0x38
#define KEY_CAPS_LOCK   0x3A
#define KEY_F1          0x3B
#define KEY_F2          0x3C
#define KEY_F3          0x3D
#define KEY_F4          0x3E
#define KEY_F5          0x3F
#define KEY_F6          0x40
#define KEY_F7          0x41
#define KEY_F8          0x42
#define KEY_F9          0x43
#define KEY_F10         0x44
#define KEY_F11         0x57
#define KEY_F12         0x58

/* Key states */
typedef struct {
    bool shift;
    bool ctrl;
    bool alt;
    bool caps_lock;
} key_state_t;

/* Initialize keyboard driver */
void keyboard_init(void);

/* Get keyboard character (blocking) */
char keyboard_getchar(void);

/* Get keyboard character (non-blocking, returns 0 if no key) */
char keyboard_getchar_nonblock(void);

/* Check if a key is available */
bool keyboard_has_key(void);

/* Get current key state */
key_state_t keyboard_get_state(void);

/* Read scan code from keyboard */
uint8_t keyboard_read_scancode(void);

#endif /* KEYBOARD_H */
