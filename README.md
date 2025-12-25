# NightOS

<p align="center">
  <img src="docs/logo.png" alt="NightOS Logo" width="200">
</p>

<p align="center">
  <strong>A minimal, dark-themed operating system</strong><br>
  Inspired by Unix/Linux with elegant simplicity
</p>

<p align="center">
  <a href="#features">Features</a> •
  <a href="#quick-start">Quick Start</a> •
  <a href="#building">Building</a> •
  <a href="#architecture">Architecture</a> •
  <a href="#contributing">Contributing</a>
</p>

---

## Overview

**NightOS** is a minimal operating system written from scratch in C and Assembly. It features a dark, elegant theme and draws inspiration from Unix/Linux philosophy while incorporating modern design sensibilities.

This project is designed for:

- 🎓 Learning OS development
- 🔬 Experimentation with low-level programming
- 🎨 Demonstrating clean, minimal OS design

## Features

### Current Features (v0.2.0 "Eclipse")

- ✅ Custom bootloader (16-bit → 32-bit protected mode)
- ✅ VGA text mode driver with full color support
- ✅ PS/2 keyboard driver
- ✅ Interactive shell with built-in commands
- ✅ Dark theme UI
- ✅ Clean, modular codebase
- ✅ Interrupt Descriptor Table (IDT)
- ✅ CPU exception handling
- ✅ Hardware interrupt support (PIC)
- ✅ PIT Timer driver (system clock)
- ✅ Real-Time Clock (RTC) driver
- ✅ Memory management (heap allocator)
- ✅ Text User Interface (TUI) framework
- ✅ RAM-based filesystem (64 files, 4KB each)
- ✅ Process management (16 processes, cooperative multitasking)
- ✅ System calls (INT 0x80 interface)
- ✅ GUI Desktop Environment (text-mode)

### Built-in Commands

| Command   | Description                |
| --------- | -------------------------- |
| `help`    | Display available commands |
| `clear`   | Clear the screen           |
| `echo`    | Print text to screen       |
| `version` | Display OS version         |
| `reboot`  | Restart the system         |
| `halt`    | Halt the system            |
| `time`    | Display system date/time   |
| `uptime`  | Show system uptime         |
| `mem`     | Display memory statistics  |
| `sleep`   | Sleep for N seconds        |
| `demo`    | TUI demonstration          |
| `gui`     | Launch desktop environment |
| `ls`      | List files in filesystem   |
| `touch`   | Create a new file          |
| `rm`      | Delete a file              |
| `cat`     | Display file contents      |
| `write`   | Write text to a file       |
| `ps`      | List running processes     |

### Planned Features

- [x] Interrupt handling (IDT)
- [x] Timer driver (PIT)
- [x] Real-time clock (RTC)
- [x] Memory management (heap allocator)
- [x] Text UI framework
- [x] Basic filesystem
- [x] Process management
- [x] System calls
- [x] GUI Desktop Environment

### GUI Desktop Features

The desktop environment (`gui` command) provides:
- 🖥️ Dark-themed desktop with wallpaper pattern
- 📊 Taskbar with Start button and system clock
- 🪟 Window management (drag, focus, close)
- 📁 Desktop icons (Terminal, Files, System Info, Settings, About)
- 📋 Start menu with application launcher
- 🕐 Real-time clock display

## Quick Start

### Prerequisites

You'll need the following tools:

- **NASM** - Assembler for bootloader and kernel entry
- **i686-elf-gcc** - Cross-compiler for 32-bit x86
- **QEMU** - Emulator for testing (recommended)

### Installation

#### Windows

1. Install [NASM](https://nasm.us/)
2. Download [i686-elf cross-compiler](https://wiki.osdev.org/GCC_Cross-Compiler)
3. Install [QEMU](https://www.qemu.org/download/)
4. Add all tools to your PATH

#### Linux (Ubuntu/Debian)

```bash
# Install NASM and QEMU
sudo apt install nasm qemu-system-x86

# Build cross-compiler (see docs/CROSS_COMPILER.md for details)
# Or install prebuilt: sudo apt install gcc-i686-linux-gnu
```

#### macOS

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install tools
brew install nasm qemu
brew install i686-elf-gcc  # From homebrew tap
```

### Building

#### Windows

```batch
build.bat
```

#### Linux/macOS

```bash
chmod +x build.sh
./build.sh
```

Or using Make:

```bash
make
```

### Running

```bash
# Using QEMU
qemu-system-i386 -drive format=raw,file=build/nightos.img

# With debug output
qemu-system-i386 -drive format=raw,file=build/nightos.img -monitor stdio
```

## Architecture

### Project Structure

```
NightOS/
├── boot/               # Bootloader
│   └── boot.asm        # 16-bit bootloader → 32-bit switch
├── kernel/             # Kernel source
│   ├── kernel_entry.asm # Entry point (ASM → C bridge)
│   ├── kernel.c        # Main kernel code
│   ├── shell.c         # Interactive shell
│   ├── idt.c           # Interrupt Descriptor Table
│   ├── isr.asm         # Interrupt Service Routines
│   ├── fs.c            # RAM filesystem
│   ├── process.c       # Process manager
│   ├── syscall.c       # System call handlers
│   └── gui.c           # Desktop environment
├── drivers/            # Hardware drivers
│   ├── vga.c           # VGA text mode driver
│   ├── keyboard.c      # PS/2 keyboard driver
│   ├── pic.c           # Programmable Interrupt Controller
│   ├── timer.c         # PIT timer driver
│   └── rtc.c           # Real-Time Clock driver
├── lib/                # Runtime libraries
│   ├── string.c        # String manipulation
│   ├── memory.c        # Heap allocator (kmalloc/kfree)
│   └── tui.c           # Text User Interface framework
├── include/            # Header files
│   ├── types.h         # Type definitions
│   ├── config.h        # OS configuration
│   ├── io.h            # Port I/O operations
│   ├── vga.h           # VGA driver header
│   ├── keyboard.h      # Keyboard driver header
│   ├── shell.h         # Shell header
│   ├── string.h        # String library header
│   ├── idt.h           # IDT header
│   ├── timer.h         # Timer header
│   ├── rtc.h           # RTC header
│   ├── memory.h        # Memory manager header
│   ├── tui.h           # TUI framework header
│   ├── fs.h            # Filesystem header
│   ├── process.h       # Process manager header
│   ├── syscall.h       # System calls header
│   └── gui.h           # GUI desktop header
├── docs/               # Documentation
├── build/              # Build output (generated)
├── Makefile            # Build system (Linux/macOS)
├── build_mingw.bat     # Build script (Windows/MinGW)
├── build.bat           # Build script (Windows/cross-compiler)
├── build.sh            # Build script (Linux/macOS)
├── linker.ld           # Linker script
└── README.md           # This file
```

### Memory Layout

```
0x00000000 ┌─────────────────────┐
           │  BIOS/Reserved      │
0x00007C00 ├─────────────────────┤
           │  Bootloader (512B)  │
0x00007E00 ├─────────────────────┤
           │  Free Memory        │
0x00001000 ├─────────────────────┤
           │  Kernel             │
           │  (Code + Data)      │
0x00090000 ├─────────────────────┤
           │  Stack              │
           │  (grows down)       │
0x000B8000 ├─────────────────────┤
           │  VGA Text Buffer    │
0x000C0000 ├─────────────────────┤
           │  BIOS ROM           │
0x00100000 ├─────────────────────┤
           │  Kernel Heap (1MB)  │
           │  (kmalloc/kfree)    │
0x00200000 └─────────────────────┘
```

### Boot Process

1. **BIOS** loads bootloader from first sector to `0x7C00`
2. **Bootloader** sets up segments, stack
3. **Bootloader** loads kernel from disk to `0x1000`
4. **Bootloader** switches CPU to 32-bit protected mode
5. **Kernel entry** (ASM) calls `kernel_main()`
6. **Kernel** initializes drivers and starts shell

## Development

### Adding New Commands

Edit `kernel/shell.c`:

```c
// 1. Add handler function
void cmd_mycommand(int argc, char* argv[]) {
    vga_puts("Hello from my command!\n");
}

// 2. Register in shell_init()
shell_register_command("mycommand", "Description here", cmd_mycommand);
```

### Adding New Drivers

1. Create header in `include/mydriver.h`
2. Create implementation in `drivers/mydriver.c`
3. Add to build system (Makefile/build scripts)
4. Initialize in `kernel_init()`

## Technical Details

### Toolchain

- **Assembler**: NASM (Netwide Assembler)
- **Compiler**: GCC cross-compiler (i686-elf-gcc)
- **Linker**: GNU LD (i686-elf-ld)
- **Target**: i386 (32-bit x86)

### Conventions

- **Coding Style**: K&R style, 4-space indentation
- **Naming**: `snake_case` for functions/variables
- **Headers**: Include guards with `_H` suffix

## Testing

### QEMU (Recommended)

```bash
# Standard run
make run

# With debug console
make debug
```

### VirtualBox

1. Create new VM (Type: Other, Version: Other/Unknown)
2. RAM: 32MB minimum
3. No hard disk needed
4. Settings → Storage → Add floppy controller
5. Add `nightos.img` as floppy disk
6. Start VM

### Bochs

Create `bochsrc.txt`:

```
megs: 32
romimage: file=$BXSHARE/BIOS-bochs-latest
vgaromimage: file=$BXSHARE/VGABIOS-lgpl-latest
floppya: 1_44=build/nightos.img, status=inserted
boot: floppy
```

Run: `bochs -f bochsrc.txt`

## Contributing

Contributions are welcome! Please read our contributing guidelines first.

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## Resources

### Learning OS Development

- [OSDev Wiki](https://wiki.osdev.org/) - Comprehensive OS development resource
- [Writing a Simple Operating System from Scratch](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf) - Nick Blundell
- [The Little OS Book](https://littleosbook.github.io/) - Erik Helin & Adam Renberg

### Related Projects

- [Linux](https://github.com/torvalds/linux) - The Linux kernel
- [Minix](https://www.minix3.org/) - Educational microkernel OS
- [xv6](https://github.com/mit-pdos/xv6-public) - MIT teaching OS

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Unix/Linux for the inspiration
- OSDev community for invaluable resources
- All contributors and supporters

---

<p align="center">
  <strong>NightOS</strong> - Simple. Dark. Elegant.
</p>
