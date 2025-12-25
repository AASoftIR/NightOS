@echo off
REM NightOS Build Script for Windows
REM Requires: NASM, i686-elf-gcc (cross compiler), QEMU

setlocal enabledelayedexpansion

REM Configuration
set ASM=nasm
set CC=i686-elf-gcc
set LD=i686-elf-ld
set QEMU=qemu-system-i386

REM Directories
set BUILD_DIR=build
set BOOT_DIR=boot
set KERNEL_DIR=kernel
set DRIVERS_DIR=drivers
set LIB_DIR=lib

REM Create build directory
if not exist %BUILD_DIR% mkdir %BUILD_DIR%

echo ============================================
echo  NightOS Build System
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
    echo ERROR: i686-elf-gcc cross compiler not found!
    echo Please install the i686-elf cross compiler toolchain.
    echo.
    echo You can build it from source or download prebuilt binaries.
    echo See: https://wiki.osdev.org/GCC_Cross-Compiler
    exit /b 1
)

echo [1/6] Assembling bootloader...
%ASM% -f bin %BOOT_DIR%\boot.asm -o %BUILD_DIR%\boot.bin
if %ERRORLEVEL% neq 0 (
    echo ERROR: Bootloader assembly failed!
    exit /b 1
)

echo [2/6] Assembling kernel entry...
%ASM% -f elf32 %KERNEL_DIR%\kernel_entry.asm -o %BUILD_DIR%\kernel_entry.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: Kernel entry assembly failed!
    exit /b 1
)

echo [3/6] Compiling kernel...
%CC% -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -nostdinc -fno-builtin -Wall -Wextra -I include -c %KERNEL_DIR%\kernel.c -o %BUILD_DIR%\kernel.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: Kernel compilation failed!
    exit /b 1
)

%CC% -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -nostdinc -fno-builtin -Wall -Wextra -I include -c %KERNEL_DIR%\shell.c -o %BUILD_DIR%\shell.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: Shell compilation failed!
    exit /b 1
)

echo [4/6] Compiling drivers...
%CC% -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -nostdinc -fno-builtin -Wall -Wextra -I include -c %DRIVERS_DIR%\vga.c -o %BUILD_DIR%\vga.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: VGA driver compilation failed!
    exit /b 1
)

%CC% -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -nostdinc -fno-builtin -Wall -Wextra -I include -c %DRIVERS_DIR%\keyboard.c -o %BUILD_DIR%\keyboard.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: Keyboard driver compilation failed!
    exit /b 1
)

echo [5/6] Compiling libraries...
%CC% -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -nostdinc -fno-builtin -Wall -Wextra -I include -c %LIB_DIR%\string.c -o %BUILD_DIR%\string.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: String library compilation failed!
    exit /b 1
)

echo [6/6] Linking kernel...
%LD% -m elf_i386 -T linker.ld --oformat binary -o %BUILD_DIR%\kernel.bin %BUILD_DIR%\kernel_entry.o %BUILD_DIR%\kernel.o %BUILD_DIR%\shell.o %BUILD_DIR%\vga.o %BUILD_DIR%\keyboard.o %BUILD_DIR%\string.o
if %ERRORLEVEL% neq 0 (
    echo ERROR: Kernel linking failed!
    exit /b 1
)

echo.
echo Creating OS image...
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
)

exit /b 0
