#!/bin/bash
# NightOS Build Script for Linux/macOS
# Requires: NASM, i686-elf-gcc (cross compiler), QEMU

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
ASM="nasm"
CC="i686-elf-gcc"
LD="i686-elf-ld"
QEMU="qemu-system-i386"

# Directories
BUILD_DIR="build"
BOOT_DIR="boot"
KERNEL_DIR="kernel"
DRIVERS_DIR="drivers"
LIB_DIR="lib"

# Compiler flags
CFLAGS="-m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -nostdinc -fno-builtin -Wall -Wextra -I include"

echo -e "${CYAN}============================================${NC}"
echo -e "${CYAN} NightOS Build System${NC}"
echo -e "${CYAN}============================================${NC}"
echo

# Create build directory
mkdir -p $BUILD_DIR

# Check for required tools
check_tool() {
    if ! command -v $1 &> /dev/null; then
        echo -e "${RED}ERROR: $1 not found!${NC}"
        echo "$2"
        exit 1
    fi
}

check_tool "$ASM" "Please install NASM: sudo apt install nasm (or brew install nasm)"
check_tool "$CC" "Please install i686-elf cross compiler. See: https://wiki.osdev.org/GCC_Cross-Compiler"

echo -e "[1/6] Assembling bootloader..."
$ASM -f bin $BOOT_DIR/boot.asm -o $BUILD_DIR/boot.bin

echo -e "[2/6] Assembling kernel entry..."
$ASM -f elf32 $KERNEL_DIR/kernel_entry.asm -o $BUILD_DIR/kernel_entry.o

echo -e "[3/6] Compiling kernel..."
$CC $CFLAGS -c $KERNEL_DIR/kernel.c -o $BUILD_DIR/kernel.o
$CC $CFLAGS -c $KERNEL_DIR/shell.c -o $BUILD_DIR/shell.o

echo -e "[4/6] Compiling drivers..."
$CC $CFLAGS -c $DRIVERS_DIR/vga.c -o $BUILD_DIR/vga.o
$CC $CFLAGS -c $DRIVERS_DIR/keyboard.c -o $BUILD_DIR/keyboard.o

echo -e "[5/6] Compiling libraries..."
$CC $CFLAGS -c $LIB_DIR/string.c -o $BUILD_DIR/string.o

echo -e "[6/6] Linking kernel..."
$LD -m elf_i386 -T linker.ld --oformat binary -o $BUILD_DIR/kernel.bin \
    $BUILD_DIR/kernel_entry.o \
    $BUILD_DIR/kernel.o \
    $BUILD_DIR/shell.o \
    $BUILD_DIR/vga.o \
    $BUILD_DIR/keyboard.o \
    $BUILD_DIR/string.o

echo
echo "Creating OS image..."
cat $BUILD_DIR/boot.bin $BUILD_DIR/kernel.bin > $BUILD_DIR/nightos.img

# Pad to sector boundary
FILESIZE=$(stat -f%z "$BUILD_DIR/nightos.img" 2>/dev/null || stat -c%s "$BUILD_DIR/nightos.img")
REMAINDER=$((FILESIZE % 512))
if [ $REMAINDER -ne 0 ]; then
    PADDING=$((512 - REMAINDER))
    dd if=/dev/zero bs=1 count=$PADDING >> $BUILD_DIR/nightos.img 2>/dev/null
fi

echo
echo -e "${GREEN}============================================${NC}"
echo -e "${GREEN} Build successful!${NC}"
echo -e "${GREEN} Output: $BUILD_DIR/nightos.img${NC}"
echo -e "${GREEN}============================================${NC}"
echo
echo "To run in QEMU: qemu-system-i386 -drive format=raw,file=$BUILD_DIR/nightos.img"
echo

# Offer to run in QEMU
if command -v $QEMU &> /dev/null; then
    read -p "Run in QEMU? (y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Starting QEMU..."
        $QEMU -drive format=raw,file=$BUILD_DIR/nightos.img
    fi
fi
