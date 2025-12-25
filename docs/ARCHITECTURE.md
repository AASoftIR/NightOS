# NightOS Architecture

This document describes the internal architecture of NightOS.

## Overview

NightOS follows a monolithic kernel design with clear separation between components. While simple, the architecture is designed to be extensible.

```
┌─────────────────────────────────────────────────────────────┐
│                      User Space (Future)                     │
├─────────────────────────────────────────────────────────────┤
│                         Shell                                │
├──────────────────┬──────────────────┬───────────────────────┤
│   VGA Driver     │  Keyboard Driver │   Future Drivers      │
├──────────────────┴──────────────────┴───────────────────────┤
│                      Kernel Core                             │
├─────────────────────────────────────────────────────────────┤
│                   Hardware Abstraction                       │
├─────────────────────────────────────────────────────────────┤
│                      Hardware                                │
└─────────────────────────────────────────────────────────────┘
```

## Boot Process

### Stage 1: BIOS

1. Computer powers on
2. BIOS performs POST (Power-On Self-Test)
3. BIOS loads first 512 bytes from boot device to `0x7C00`
4. BIOS transfers control to `0x7C00`

### Stage 2: Bootloader

Located in `boot/boot.asm`:

1. **Real Mode Setup** (16-bit)

   - Set up segment registers
   - Initialize stack at `0x9000`
   - Save boot drive number

2. **Kernel Loading**

   - Use BIOS INT 13h to read disk
   - Load kernel to `0x1000`
   - Read 15 sectors (7.5KB max kernel size)

3. **Protected Mode Switch**

   - Disable interrupts
   - Load GDT (Global Descriptor Table)
   - Enable A20 line (if needed)
   - Set PE bit in CR0
   - Far jump to flush pipeline

4. **32-bit Initialization**
   - Set up segment registers for protected mode
   - Initialize stack
   - Jump to kernel

### Stage 3: Kernel Entry

Located in `kernel/kernel_entry.asm`:

1. Provides bridge between assembly and C
2. Calls `kernel_main()` function
3. Halts if kernel returns

### Stage 4: Kernel Initialization

Located in `kernel/kernel.c`:

1. Initialize VGA driver
2. Initialize keyboard driver
3. Initialize shell
4. Display boot logo
5. Enter shell main loop

## Memory Map

```
Address         Size    Description
─────────────────────────────────────────
0x00000000      1KB     Real Mode IVT (Interrupt Vector Table)
0x00000400      256B    BIOS Data Area
0x00000500      ~30KB   Free (conventional memory)
0x00007C00      512B    Bootloader
0x00007E00      ~480KB  Free (conventional memory)
0x00001000      ~32KB   Kernel code and data
0x00080000      ~64KB   Extended BIOS Data Area
0x00090000      64KB    Stack (grows down)
0x000A0000      64KB    VGA Video Memory
0x000B8000      32KB    VGA Text Mode Buffer
0x000C0000      256KB   BIOS ROM
0x00100000      ...     Extended Memory (future use)
```

## Component Details

### VGA Text Mode Driver

**Location**: `drivers/vga.c`

**Features**:

- 80x25 character display
- 16 foreground colors
- 8 background colors
- Hardware cursor control
- Scrolling support
- Printf-style formatting

**Color Palette**:

```
0  - Black           8  - Dark Grey
1  - Blue            9  - Light Blue
2  - Green           10 - Light Green
3  - Cyan            11 - Light Cyan
4  - Red             12 - Light Red
5  - Magenta         13 - Light Magenta
6  - Brown           14 - Yellow
7  - Light Grey      15 - White
```

### Keyboard Driver

**Location**: `drivers/keyboard.c`

**Features**:

- PS/2 keyboard support
- Scan code set 1 translation
- US QWERTY layout
- Modifier key tracking (Shift, Ctrl, Alt, Caps Lock)
- Blocking and non-blocking input

**Scan Code Processing**:

```
1. Read scan code from port 0x60
2. Check for key release (bit 7)
3. Handle modifier keys
4. Translate to ASCII using lookup table
5. Apply shift/caps lock modifiers
6. Return character
```

### Shell

**Location**: `kernel/shell.c`

**Features**:

- Command line input
- Argument parsing
- Extensible command system
- Color-coded output

**Command Registration**:

```c
typedef void (*command_handler_t)(int argc, char* argv[]);

typedef struct {
    const char* name;
    const char* description;
    command_handler_t handler;
} shell_command_t;
```

## Global Descriptor Table (GDT)

The GDT defines memory segments for protected mode:

| Entry | Base | Limit | Type        | DPL |
| ----- | ---- | ----- | ----------- | --- |
| 0     | -    | -     | Null        | -   |
| 1     | 0    | 4GB   | Code (exec) | 0   |
| 2     | 0    | 4GB   | Data (r/w)  | 0   |

Using a flat memory model (all segments span entire address space).

## Future Architecture Plans

### Interrupt Descriptor Table (IDT)

```c
typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t offset_high;
} __attribute__((packed)) idt_entry_t;
```

### Memory Management

Planned features:

- Physical memory manager (bitmap allocator)
- Virtual memory (paging)
- Kernel heap allocator

### Process Management

Planned features:

- Process Control Block (PCB)
- Context switching
- Round-robin scheduling

## Coding Conventions

### Naming

- Functions: `module_action()` (e.g., `vga_putchar()`)
- Types: `name_t` suffix (e.g., `vga_color_t`)
- Macros: `UPPER_CASE` (e.g., `VGA_WIDTH`)
- Globals: Prefixed with module name

### File Organization

```c
/*
 * Module description
 */

#include "headers.h"

// Macros and constants

// Type definitions

// Static (private) variables

// Static (private) functions

// Public functions
```

### Error Handling

- Return negative values for errors
- Use `bool` for success/failure when appropriate
- Critical errors: halt system with message

## References

- Intel 64 and IA-32 Architectures Software Developer Manual
- OSDev Wiki (wiki.osdev.org)
- "Operating Systems: Three Easy Pieces" by Remzi H. Arpaci-Dusseau
