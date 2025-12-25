@echo off
REM NightOS Build Script for Windows (Using MinGW GCC)
REM This version uses regular MinGW GCC with cross-compilation flags

setlocal enabledelayedexpansion

REM Configuration - Using MinGW GCC instead of cross-compiler
set ASM=nasm
set CC=gcc
set LD=ld
set OBJCOPY=objcopy
set QEMU=qemu-system-i386

REM Directories
set BUILD_DIR=build
set BOOT_DIR=boot
set KERNEL_DIR=kernel
set DRIVERS_DIR=drivers
set LIB_DIR=lib

REM Compiler flags for freestanding 32-bit code
set CFLAGS=-m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -fno-builtin -Wall -Wextra -I include -c

REM Create build directory
if not exist %BUILD_DIR% mkdir %BUILD_DIR%

echo ============================================
echo  NightOS Build System (MinGW Version)
echo ============================================
echo.

REM Check for required tools
where %ASM% >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: NASM assembler not found!
    echo Please install NASM and add it to PATH.
    exit /b 1
)

where %CC% >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: GCC compiler not found!
    echo Please install MinGW-w64 and add it to PATH.
    exit /b 1
)

echo [1/7] Assembling bootloader...
%ASM% -f bin %BOOT_DIR%\boot.asm -o %BUILD_DIR%\boot.bin
if %ERRORLEVEL% neq 0 (
    echo ERROR: Bootloader assembly failed!
    exit /b 1
)

echo [2/9] Assembling kernel entry...
%ASM% -f elf32 -DMINGW %KERNEL_DIR%\kernel_entry.asm -o %BUILD_DIR%\kernel_entry.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: Kernel entry assembly failed!
    exit /b 1
)

echo [3/9] Assembling ISR handlers...
%ASM% -f elf32 -DMINGW %KERNEL_DIR%\isr.asm -o %BUILD_DIR%\isr.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: ISR assembly failed!
    exit /b 1
)

echo [4/9] Compiling kernel...
%CC% %CFLAGS% %KERNEL_DIR%\kernel.c -o %BUILD_DIR%\kernel.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: Kernel compilation failed!
    exit /b 1
)

%CC% %CFLAGS% %KERNEL_DIR%\shell.c -o %BUILD_DIR%\shell.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: Shell compilation failed!
    exit /b 1
)

%CC% %CFLAGS% %KERNEL_DIR%\idt.c -o %BUILD_DIR%\idt.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: IDT compilation failed!
    exit /b 1
)

%CC% %CFLAGS% %KERNEL_DIR%\fs.c -o %BUILD_DIR%\fs.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: Filesystem compilation failed!
    exit /b 1
)

%CC% %CFLAGS% %KERNEL_DIR%\process.c -o %BUILD_DIR%\process.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: Process compilation failed!
    exit /b 1
)

%CC% %CFLAGS% %KERNEL_DIR%\syscall.c -o %BUILD_DIR%\syscall.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: Syscall compilation failed!
    exit /b 1
)

%CC% %CFLAGS% %KERNEL_DIR%\gui.c -o %BUILD_DIR%\gui.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: GUI compilation failed!
    exit /b 1
)

echo [5/9] Compiling drivers...
%CC% %CFLAGS% %DRIVERS_DIR%\vga.c -o %BUILD_DIR%\vga.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: VGA driver compilation failed!
    exit /b 1
)

%CC% %CFLAGS% %DRIVERS_DIR%\keyboard.c -o %BUILD_DIR%\keyboard.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: Keyboard driver compilation failed!
    exit /b 1
)

%CC% %CFLAGS% %DRIVERS_DIR%\pic.c -o %BUILD_DIR%\pic.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: PIC driver compilation failed!
    exit /b 1
)

%CC% %CFLAGS% %DRIVERS_DIR%\timer.c -o %BUILD_DIR%\timer.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: Timer driver compilation failed!
    exit /b 1
)

%CC% %CFLAGS% %DRIVERS_DIR%\rtc.c -o %BUILD_DIR%\rtc.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: RTC driver compilation failed!
    exit /b 1
)

echo [6/9] Compiling libraries...
%CC% %CFLAGS% %LIB_DIR%\string.c -o %BUILD_DIR%\string.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: String library compilation failed!
    exit /b 1
)

%CC% %CFLAGS% %LIB_DIR%\memory.c -o %BUILD_DIR%\memory.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: Memory library compilation failed!
    exit /b 1
)

%CC% %CFLAGS% %LIB_DIR%\tui.c -o %BUILD_DIR%\tui.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: TUI library compilation failed!
    exit /b 1
)

echo [7/9] Linking kernel...
%LD% -m i386pe -e _start -Ttext 0x1000 -o %BUILD_DIR%\kernel.pe %BUILD_DIR%\kernel_entry.o %BUILD_DIR%\isr.o %BUILD_DIR%\kernel.o %BUILD_DIR%\shell.o %BUILD_DIR%\idt.o %BUILD_DIR%\fs.o %BUILD_DIR%\process.o %BUILD_DIR%\syscall.o %BUILD_DIR%\gui.o %BUILD_DIR%\vga.o %BUILD_DIR%\keyboard.o %BUILD_DIR%\pic.o %BUILD_DIR%\timer.o %BUILD_DIR%\rtc.o %BUILD_DIR%\string.o %BUILD_DIR%\memory.o %BUILD_DIR%\tui.o 2>nul

REM Convert PE to raw binary
echo [8/9] Converting to binary format...
%OBJCOPY% -O binary %BUILD_DIR%\kernel.pe %BUILD_DIR%\kernel.bin
if %ERRORLEVEL% neq 0 (
    echo ERROR: Binary conversion failed!
    exit /b 1
)

echo [9/9] Creating OS image...
copy /b %BUILD_DIR%\boot.bin + %BUILD_DIR%\kernel.bin %BUILD_DIR%\nightos.img >nul
if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to create OS image!
    exit /b 1
)

echo.
echo ============================================
echo  Build successful!
echo  Output: %BUILD_DIR%\nightos.img
echo ============================================
echo.
echo To run in QEMU: qemu-system-i386 -drive format=raw,file=%BUILD_DIR%\nightos.img
echo.

REM Check if QEMU is available and offer to run
where %QEMU% >nul 2>&1
if %ERRORLEVEL% equ 0 (
    set /p RUN_QEMU="Run in QEMU? (y/n): "
    if /i "!RUN_QEMU!"=="y" (
        echo.
        echo Starting QEMU...
        %QEMU% -drive format=raw,file=%BUILD_DIR%\nightos.img
    )
) else (
    echo QEMU not found. Install QEMU to test the OS.
    echo Download from: https://www.qemu.org/download/
)

exit /b 0
