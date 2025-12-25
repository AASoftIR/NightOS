# NightOS API Reference

This document provides API documentation for NightOS components.

## Table of Contents

- [VGA Driver](#vga-driver)
- [Keyboard Driver](#keyboard-driver)
- [Shell](#shell)
- [String Library](#string-library)
- [I/O Operations](#io-operations)

---

## VGA Driver

**Header**: `include/vga.h`  
**Source**: `drivers/vga.c`

### Types

#### `vga_color_t`

Enumeration of VGA text mode colors.

```c
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
```

### Functions

#### `vga_init`

Initialize the VGA driver.

```c
void vga_init(void);
```

**Description**: Clears the screen, sets default colors (light grey on black), and enables the cursor.

---

#### `vga_clear`

Clear the screen.

```c
void vga_clear(void);
```

**Description**: Fills the screen with spaces using the current color and resets cursor to (0, 0).

---

#### `vga_set_color`

Set the current text color.

```c
void vga_set_color(uint8_t color);
```

**Parameters**:

- `color`: Combined foreground/background color attribute

**Example**:

```c
vga_set_color(vga_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE));
```

---

#### `vga_color`

Create a color attribute from foreground and background colors.

```c
static inline uint8_t vga_color(vga_color_t fg, vga_color_t bg);
```

**Parameters**:

- `fg`: Foreground color
- `bg`: Background color

**Returns**: Combined color attribute byte

---

#### `vga_putchar`

Print a single character.

```c
void vga_putchar(char c);
```

**Parameters**:

- `c`: Character to print

**Special Characters**:

- `\n`: Newline
- `\r`: Carriage return
- `\t`: Tab (8 spaces)
- `\b`: Backspace

---

#### `vga_puts`

Print a null-terminated string.

```c
void vga_puts(const char* str);
```

**Parameters**:

- `str`: Null-terminated string to print

---

#### `vga_printf`

Formatted print (simplified printf).

```c
void vga_printf(const char* format, ...);
```

**Format Specifiers**:

- `%d`, `%i`: Signed decimal integer
- `%u`: Unsigned decimal integer
- `%x`: Hexadecimal (lowercase)
- `%s`: String
- `%c`: Character
- `%%`: Literal percent sign

**Example**:

```c
vga_printf("Value: %d, Hex: 0x%x\n", 42, 255);
```

---

#### `vga_set_cursor`

Set cursor position.

```c
void vga_set_cursor(int x, int y);
```

**Parameters**:

- `x`: Column (0-79)
- `y`: Row (0-24)

---

## Keyboard Driver

**Header**: `include/keyboard.h`  
**Source**: `drivers/keyboard.c`

### Types

#### `key_state_t`

Structure holding modifier key states.

```c
typedef struct {
    bool shift;
    bool ctrl;
    bool alt;
    bool caps_lock;
} key_state_t;
```

### Functions

#### `keyboard_init`

Initialize the keyboard driver.

```c
void keyboard_init(void);
```

**Description**: Clears the keyboard buffer and resets modifier states.

---

#### `keyboard_getchar`

Get a character from keyboard (blocking).

```c
char keyboard_getchar(void);
```

**Returns**: ASCII character pressed

**Description**: Waits until a key is pressed and returns the corresponding character.

---

#### `keyboard_getchar_nonblock`

Get a character from keyboard (non-blocking).

```c
char keyboard_getchar_nonblock(void);
```

**Returns**: ASCII character, or 0 if no key pressed

---

#### `keyboard_has_key`

Check if a key is available.

```c
bool keyboard_has_key(void);
```

**Returns**: `true` if a key is waiting, `false` otherwise

---

#### `keyboard_get_state`

Get current modifier key states.

```c
key_state_t keyboard_get_state(void);
```

**Returns**: Current state of modifier keys

---

## Shell

**Header**: `include/shell.h`  
**Source**: `kernel/shell.c`

### Types

#### `command_handler_t`

Command handler function pointer type.

```c
typedef void (*command_handler_t)(int argc, char* argv[]);
```

### Functions

#### `shell_init`

Initialize the shell.

```c
void shell_init(void);
```

**Description**: Registers built-in commands and prepares the shell for use.

---

#### `shell_run`

Start the shell main loop.

```c
void shell_run(void);
```

**Description**: Enters an infinite loop processing user input. Does not return.

---

#### `shell_register_command`

Register a new command.

```c
void shell_register_command(const char* name, const char* desc, command_handler_t handler);
```

**Parameters**:

- `name`: Command name (what user types)
- `desc`: Command description (shown in help)
- `handler`: Function to call when command is executed

**Example**:

```c
void cmd_hello(int argc, char* argv[]) {
    vga_puts("Hello, World!\n");
}

shell_register_command("hello", "Say hello", cmd_hello);
```

---

## String Library

**Header**: `include/string.h`  
**Source**: `lib/string.c`

### Functions

#### `strlen`

Get string length.

```c
size_t strlen(const char* str);
```

---

#### `strcpy`

Copy string.

```c
char* strcpy(char* dest, const char* src);
```

---

#### `strncpy`

Copy string with limit.

```c
char* strncpy(char* dest, const char* src, size_t n);
```

---

#### `strcmp`

Compare strings.

```c
int strcmp(const char* s1, const char* s2);
```

**Returns**: 0 if equal, <0 if s1 < s2, >0 if s1 > s2

---

#### `memset`

Fill memory with value.

```c
void* memset(void* ptr, int value, size_t num);
```

---

#### `memcpy`

Copy memory.

```c
void* memcpy(void* dest, const void* src, size_t num);
```

---

#### `itoa`

Convert integer to string.

```c
void itoa(int value, char* str, int base);
```

**Parameters**:

- `value`: Integer to convert
- `str`: Output buffer
- `base`: Numeric base (e.g., 10 for decimal, 16 for hex)

---

## I/O Operations

**Header**: `include/io.h`

### Functions

#### `inb`

Read byte from I/O port.

```c
static inline uint8_t inb(uint16_t port);
```

---

#### `outb`

Write byte to I/O port.

```c
static inline void outb(uint16_t port, uint8_t value);
```

---

#### `inw`

Read word from I/O port.

```c
static inline uint16_t inw(uint16_t port);
```

---

#### `outw`

Write word to I/O port.

```c
static inline void outw(uint16_t port, uint16_t value);
```

---

#### `io_wait`

Small I/O delay.

```c
static inline void io_wait(void);
```

**Description**: Writes to unused port 0x80 to create a small delay, useful after some I/O operations.
