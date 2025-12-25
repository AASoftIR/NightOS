# Building the i686-elf Cross-Compiler

This guide explains how to build the i686-elf cross-compiler toolchain required for NightOS development.

## Why a Cross-Compiler?

When building an operating system, you need a compiler that:

1. Targets the correct architecture (i386/i686)
2. Doesn't link against your host OS libraries
3. Produces freestanding binaries

## Prerequisites

### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev \
                 libmpfr-dev texinfo libisl-dev
```

### macOS

```bash
brew install gmp mpfr libmpc
xcode-select --install
```

### Windows

Use WSL (Windows Subsystem for Linux) or Cygwin with the required packages.

## Building the Toolchain

### Step 1: Set Environment Variables

```bash
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
```

Add to `~/.bashrc` for persistence:

```bash
echo 'export PREFIX="$HOME/opt/cross"' >> ~/.bashrc
echo 'export TARGET=i686-elf' >> ~/.bashrc
echo 'export PATH="$PREFIX/bin:$PATH"' >> ~/.bashrc
```

### Step 2: Download Sources

```bash
mkdir ~/src && cd ~/src

# Binutils
wget https://ftp.gnu.org/gnu/binutils/binutils-2.41.tar.xz
tar xf binutils-2.41.tar.xz

# GCC
wget https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.xz
tar xf gcc-13.2.0.tar.xz
```

### Step 3: Build Binutils

```bash
cd ~/src
mkdir build-binutils && cd build-binutils
../binutils-2.41/configure --target=$TARGET --prefix="$PREFIX" \
    --with-sysroot --disable-nls --disable-werror
make -j$(nproc)
make install
```

### Step 4: Build GCC

```bash
cd ~/src
mkdir build-gcc && cd build-gcc
../gcc-13.2.0/configure --target=$TARGET --prefix="$PREFIX" \
    --disable-nls --enable-languages=c --without-headers
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make install-gcc
make install-target-libgcc
```

### Step 5: Verify Installation

```bash
i686-elf-gcc --version
# Should output: i686-elf-gcc (GCC) 13.2.0

i686-elf-ld --version
# Should output: GNU ld (GNU Binutils) 2.41
```

## Pre-built Binaries

### Linux

Some distributions provide pre-built cross-compilers:

```bash
# Ubuntu/Debian (may not be i686-elf specifically)
sudo apt install gcc-i686-linux-gnu

# Or use this PPA for i686-elf
sudo add-apt-repository ppa:peterharrison/cross
sudo apt update
sudo apt install i686-elf-gcc
```

### macOS

Using Homebrew with a tap:

```bash
brew tap nativeos/i386-elf-toolchain
brew install i386-elf-binutils i386-elf-gcc
```

### Windows

Download pre-built binaries from:

- https://github.com/lordmilko/i686-elf-tools/releases

## Troubleshooting

### "Command not found"

Make sure the cross-compiler bin directory is in your PATH:

```bash
export PATH="$HOME/opt/cross/bin:$PATH"
```

### Build Errors

1. Ensure all dependencies are installed
2. Try with different GCC/Binutils versions
3. Check OSDev wiki for known issues

### Library Errors

If you see errors about missing libraries when compiling the kernel:

```bash
# Make sure you're using the cross-compiler, not the host compiler
which i686-elf-gcc
# Should output: /home/user/opt/cross/bin/i686-elf-gcc
```

## Resources

- [OSDev Wiki - GCC Cross-Compiler](https://wiki.osdev.org/GCC_Cross-Compiler)
- [GNU GCC Documentation](https://gcc.gnu.org/onlinedocs/)
- [GNU Binutils Documentation](https://sourceware.org/binutils/docs/)
