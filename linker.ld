/*
 * NightOS - Linker Script
 * 
 * Defines memory layout for the kernel
 */

ENTRY(_start)

SECTIONS {
    /* Kernel starts at 4KB offset (loaded by bootloader) */
    . = 0x1000;

    /* Text section (code) */
    .text : {
        *(.text)
    }

    /* Read-only data */
    .rodata : {
        *(.rodata)
    }

    /* Initialized data */
    .data : {
        *(.data)
    }

    /* Uninitialized data (BSS) */
    .bss : {
        *(COMMON)
        *(.bss)
    }
}
