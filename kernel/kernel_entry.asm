; Kernel Entry Point
; Bridge between bootloader and C kernel

[BITS 32]

; Handle different name mangling conventions
; MinGW uses underscore prefix, ELF doesn't
%ifdef MINGW
[EXTERN _kernel_main]
%define KERNEL_MAIN _kernel_main
%else
[EXTERN kernel_main]
%define KERNEL_MAIN kernel_main
%endif

global _start

_start:
    ; Call the main kernel function (C code)
    call KERNEL_MAIN
    
    ; If kernel returns, hang
    jmp $
