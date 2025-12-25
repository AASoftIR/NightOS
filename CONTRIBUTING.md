# Contributing to NightOS

First off, thank you for considering contributing to NightOS! It's people like you that make NightOS such a great learning project.

## Code of Conduct

This project and everyone participating in it is governed by our commitment to creating a welcoming and inclusive community. Please be respectful and constructive in all interactions.

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check existing issues to avoid duplicates. When creating a bug report, include as many details as possible:

- **Use a clear and descriptive title**
- **Describe the exact steps to reproduce the problem**
- **Describe the behavior you observed and what you expected**
- **Include your development environment details** (OS, compiler version, QEMU version)
- **Include relevant logs or screenshots**

### Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion:

- **Use a clear and descriptive title**
- **Provide a detailed description of the proposed feature**
- **Explain why this enhancement would be useful**
- **List any alternative solutions you've considered**

### Pull Requests

1. Fork the repo and create your branch from `main`
2. If you've added code that should be tested, add tests
3. Ensure the code compiles without warnings
4. Make sure your code follows the project's coding style
5. Write a clear commit message

## Development Setup

### Prerequisites

- NASM assembler
- i686-elf cross-compiler (see `docs/CROSS_COMPILER.md`)
- QEMU for testing
- Git

### Building

```bash
# Clone your fork
git clone https://github.com/YOUR_USERNAME/NightOS.git
cd NightOS

# Build
make

# Test in QEMU
make run
```

## Coding Style

### C Code

- Use K&R style braces
- 4-space indentation (no tabs)
- Maximum line length: 100 characters
- Use `snake_case` for functions and variables
- Use `UPPER_CASE` for macros and constants
- Add comments for complex logic

```c
/*
 * Function description
 */
void example_function(int parameter) {
    if (parameter > 0) {
        // Do something
        do_action(parameter);
    } else {
        // Handle other case
        handle_error();
    }
}
```

### Assembly Code

- Use NASM syntax
- Comment non-obvious instructions
- Group related code with section comments
- Use meaningful label names

```asm
; ============================================
; Section description
; ============================================
section_label:
    mov ax, VALUE      ; Description of operation
    int 0x10           ; BIOS interrupt
    ret
```

### Commit Messages

- Use the present tense ("Add feature" not "Added feature")
- Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
- Limit the first line to 72 characters
- Reference issues and pull requests when relevant

Example:

```
Add keyboard buffer for type-ahead support

- Implement circular buffer for keyboard input
- Add buffer overflow handling
- Update keyboard_getchar to use buffer

Fixes #42
```

## Project Structure

When adding new features:

- Headers go in `include/`
- Driver implementations go in `drivers/`
- Kernel code goes in `kernel/`
- Library code goes in `lib/`
- Documentation goes in `docs/`

## Areas for Contribution

### Good First Issues

- Improve documentation
- Add new shell commands
- Fix typos
- Add code comments

### Intermediate

- Implement RTC (Real-Time Clock) driver
- Add PIT (Programmable Interval Timer) driver
- Implement basic interrupt handling
- Add more keyboard layouts

### Advanced

- Implement memory management
- Add paging support
- Create basic filesystem
- Implement process management

## Questions?

Feel free to open an issue with your question or reach out to the maintainers.

## Recognition

Contributors will be acknowledged in:

- The README.md file
- The project's contributors page
- Release notes for significant contributions

Thank you for contributing to NightOS! 🌙
