/*
 * NightOS - Keyboard Driver Implementation
 * 
 * PS/2 keyboard driver with scan code set 1 translation
 */

#include "../include/keyboard.h"
#include "../include/io.h"

/* US keyboard layout - lowercase */
static const char scancode_to_char[] = {
    0,    0,    '1',  '2',  '3',  '4',  '5',  '6',  /* 0x00 - 0x07 */
    '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t', /* 0x08 - 0x0F */
    'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  /* 0x10 - 0x17 */
    'o',  'p',  '[',  ']',  '\n', 0,    'a',  's',  /* 0x18 - 0x1F */
    'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  /* 0x20 - 0x27 */
    '\'', '`',  0,    '\\', 'z',  'x',  'c',  'v',  /* 0x28 - 0x2F */
    'b',  'n',  'm',  ',',  '.',  '/',  0,    '*',  /* 0x30 - 0x37 */
    0,    ' ',  0,    0,    0,    0,    0,    0,    /* 0x38 - 0x3F */
    0,    0,    0,    0,    0,    0,    0,    '7',  /* 0x40 - 0x47 */
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',  /* 0x48 - 0x4F */
    '2',  '3',  '0',  '.',  0,    0,    0,    0,    /* 0x50 - 0x57 */
};

/* US keyboard layout - uppercase/shifted */
static const char scancode_to_char_shift[] = {
    0,    0,    '!',  '@',  '#',  '$',  '%',  '^',  /* 0x00 - 0x07 */
    '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t', /* 0x08 - 0x0F */
    'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  /* 0x10 - 0x17 */
    'O',  'P',  '{',  '}',  '\n', 0,    'A',  'S',  /* 0x18 - 0x1F */
    'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',  /* 0x20 - 0x27 */
    '"',  '~',  0,    '|',  'Z',  'X',  'C',  'V',  /* 0x28 - 0x2F */
    'B',  'N',  'M',  '<',  '>',  '?',  0,    '*',  /* 0x30 - 0x37 */
    0,    ' ',  0,    0,    0,    0,    0,    0,    /* 0x38 - 0x3F */
};

/* Current key state */
static key_state_t key_state = {false, false, false, false};

/* Initialize keyboard driver */
void keyboard_init(void) {
    /* Clear keyboard buffer */
    while (inb(KEYBOARD_STATUS_PORT) & 0x01) {
        inb(KEYBOARD_DATA_PORT);
    }
    
    key_state.shift = false;
    key_state.ctrl = false;
    key_state.alt = false;
    key_state.caps_lock = false;
}

/* Read raw scancode */
uint8_t keyboard_read_scancode(void) {
    while (!(inb(KEYBOARD_STATUS_PORT) & 0x01));
    return inb(KEYBOARD_DATA_PORT);
}

/* Check if key is available */
bool keyboard_has_key(void) {
    return (inb(KEYBOARD_STATUS_PORT) & 0x01) != 0;
}

/* Get current key state */
key_state_t keyboard_get_state(void) {
    return key_state;
}

/* Process scancode and return character */
static char process_scancode(uint8_t scancode) {
    bool key_released = (scancode & 0x80) != 0;
    uint8_t key = scancode & 0x7F;
    
    /* Handle modifier keys */
    switch (key) {
        case KEY_LSHIFT:
        case KEY_RSHIFT:
            key_state.shift = !key_released;
            return 0;
        case KEY_LCTRL:
            key_state.ctrl = !key_released;
            return 0;
        case KEY_LALT:
            key_state.alt = !key_released;
            return 0;
        case KEY_CAPS_LOCK:
            if (!key_released) {
                key_state.caps_lock = !key_state.caps_lock;
            }
            return 0;
    }
    
    /* Ignore key releases for regular keys */
    if (key_released) {
        return 0;
    }
    
    /* Get character from scancode table */
    if (key >= sizeof(scancode_to_char)) {
        return 0;
    }
    
    char c;
    if (key_state.shift) {
        c = scancode_to_char_shift[key];
    } else {
        c = scancode_to_char[key];
    }
    
    /* Apply caps lock for letters */
    if (key_state.caps_lock && c >= 'a' && c <= 'z') {
        c = c - 'a' + 'A';
    } else if (key_state.caps_lock && c >= 'A' && c <= 'Z') {
        c = c - 'A' + 'a';
    }
    
    return c;
}

/* Get character (blocking) */
char keyboard_getchar(void) {
    char c = 0;
    while (!c) {
        uint8_t scancode = keyboard_read_scancode();
        c = process_scancode(scancode);
    }
    return c;
}

/* Get character (non-blocking) */
char keyboard_getchar_nonblock(void) {
    if (!keyboard_has_key()) {
        return 0;
    }
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    return process_scancode(scancode);
}
