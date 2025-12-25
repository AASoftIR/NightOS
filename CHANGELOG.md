# Changelog

All notable changes to NightOS will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned

- Memory management (heap allocator)
- Interrupt handling (IDT)
- Timer driver (PIT)
- Real-time clock (RTC)
- Basic filesystem

## [0.1.0] - 2025-12-25

### Added

- Initial release of NightOS "Midnight"
- Custom bootloader with 16-bit to 32-bit protected mode switching
- VGA text mode driver with full 16-color support
- PS/2 keyboard driver with US QWERTY layout
- Interactive shell with built-in commands
- Dark theme UI design
- Build system for Windows, Linux, and macOS

### Built-in Commands

- `help` - Display available commands
- `clear` - Clear the screen
- `echo` - Print text to screen
- `version` - Display OS version information
- `reboot` - Restart the system
- `halt` - Halt the system
- `time` - Display system time (placeholder)

### Documentation

- README with quick start guide
- Architecture documentation
- API reference
- Cross-compiler build instructions

---

## Version History

| Version | Codename | Date       | Highlights                   |
| ------- | -------- | ---------- | ---------------------------- |
| 0.1.0   | Midnight | 2025-12-25 | Initial release, basic shell |
