# NightOS Makefile
# Build system for the minimal OS

# Toolchain
ASM = nasm
CC = i686-elf-gcc
LD = i686-elf-ld

# Flags
ASMFLAGS = -f bin
ASMFLAGS_ELF = -f elf32
CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -nostdinc \
         -fno-builtin -Wall -Wextra -I include
LDFLAGS = -m elf_i386 -T linker.ld --oformat binary

# Directories
BOOT_DIR = boot
KERNEL_DIR = kernel
DRIVERS_DIR = drivers
LIB_DIR = lib
BUILD_DIR = build
INCLUDE_DIR = include

# Source files
BOOT_SRC = $(BOOT_DIR)/boot.asm
KERNEL_ENTRY = $(KERNEL_DIR)/kernel_entry.asm
KERNEL_SRC = $(KERNEL_DIR)/kernel.c $(KERNEL_DIR)/shell.c
DRIVERS_SRC = $(DRIVERS_DIR)/vga.c $(DRIVERS_DIR)/keyboard.c
LIB_SRC = $(LIB_DIR)/string.c

# Object files
KERNEL_ENTRY_OBJ = $(BUILD_DIR)/kernel_entry.o
KERNEL_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(KERNEL_SRC)))
DRIVERS_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(DRIVERS_SRC)))
LIB_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(LIB_SRC)))
ALL_OBJ = $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJ) $(DRIVERS_OBJ) $(LIB_OBJ)

# Output files
BOOTLOADER = $(BUILD_DIR)/boot.bin
KERNEL = $(BUILD_DIR)/kernel.bin
OS_IMAGE = $(BUILD_DIR)/nightos.img

# Default target
all: $(BUILD_DIR) $(OS_IMAGE)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build bootloader
$(BOOTLOADER): $(BOOT_SRC)
	$(ASM) $(ASMFLAGS) $< -o $@

# Build kernel entry
$(KERNEL_ENTRY_OBJ): $(KERNEL_ENTRY)
	$(ASM) $(ASMFLAGS_ELF) $< -o $@

# Build kernel C files
$(BUILD_DIR)/kernel.o: $(KERNEL_DIR)/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/shell.o: $(KERNEL_DIR)/shell.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build driver C files
$(BUILD_DIR)/vga.o: $(DRIVERS_DIR)/vga.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/keyboard.o: $(DRIVERS_DIR)/keyboard.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build library C files
$(BUILD_DIR)/string.o: $(LIB_DIR)/string.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link kernel
$(KERNEL): $(ALL_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

# Create OS image
$(OS_IMAGE): $(BOOTLOADER) $(KERNEL)
	cat $(BOOTLOADER) $(KERNEL) > $@
	# Pad to multiple of 512 bytes (sector size)
	truncate -s %512 $@

# Run in QEMU
run: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE)

# Run with debug
debug: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE) -monitor stdio

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all clean run debug
