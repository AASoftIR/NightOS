; NightOS Bootloader
; A minimal bootloader that loads the kernel from disk

[BITS 16]
[ORG 0x7C00]

KERNEL_OFFSET equ 0x1000     ; Memory offset where we'll load the kernel

; BIOS sets boot drive in dl
mov [BOOT_DRIVE], dl

; Setup stack
mov bp, 0x9000
mov sp, bp

; Print boot message
mov si, MSG_BOOT
call print_string

; Load kernel from disk
call load_kernel

; Switch to 32-bit protected mode
call switch_to_pm

jmp $                        ; Should never reach here

; ============================================
; Print string routine (BIOS interrupt)
; ============================================
print_string:
    pusha
    mov ah, 0x0E
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    popa
    ret

; ============================================
; Load kernel from disk
; ============================================
load_kernel:
    mov si, MSG_LOAD_KERNEL
    call print_string

    ; Setup for disk read
    mov bx, KERNEL_OFFSET    ; Load to ES:BX
    mov dh, 15               ; Read 15 sectors
    mov dl, [BOOT_DRIVE]     ; Drive number
    call disk_load
    ret

; ============================================
; Disk load routine
; ============================================
disk_load:
    pusha
    push dx

    mov ah, 0x02             ; BIOS read sector function
    mov al, dh               ; Number of sectors to read
    mov cl, 0x02             ; Start from sector 2
    mov ch, 0x00             ; Cylinder 0
    mov dh, 0x00             ; Head 0

    int 0x13                 ; BIOS interrupt
    jc disk_error            ; Jump if error (CF set)

    pop dx
    cmp al, dh               ; Check sectors read
    jne sectors_error

    popa
    ret

disk_error:
    mov si, MSG_DISK_ERROR
    call print_string
    jmp $

sectors_error:
    mov si, MSG_SECTORS_ERROR
    call print_string
    jmp $

; ============================================
; GDT (Global Descriptor Table)
; ============================================
gdt_start:
    ; Null descriptor
    dq 0x0

gdt_code:
    ; Code segment descriptor
    dw 0xFFFF                ; Limit (bits 0-15)
    dw 0x0                   ; Base (bits 0-15)
    db 0x0                   ; Base (bits 16-23)
    db 10011010b             ; Access byte
    db 11001111b             ; Flags + Limit (bits 16-19)
    db 0x0                   ; Base (bits 24-31)

gdt_data:
    ; Data segment descriptor
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1   ; GDT size
    dd gdt_start                  ; GDT address

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; ============================================
; Switch to Protected Mode
; ============================================
switch_to_pm:
    cli                      ; Disable interrupts
    lgdt [gdt_descriptor]    ; Load GDT

    ; Enable protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Far jump to 32-bit code
    jmp CODE_SEG:init_pm

[BITS 32]
init_pm:
    ; Setup segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Setup stack
    mov ebp, 0x90000
    mov esp, ebp

    ; Jump to kernel
    call KERNEL_OFFSET

    jmp $                    ; Hang if kernel returns

; ============================================
; Data
; ============================================
BOOT_DRIVE:       db 0
MSG_BOOT:         db "NightOS Bootloader v0.1", 13, 10, 0
MSG_LOAD_KERNEL:  db "Loading kernel...", 13, 10, 0
MSG_DISK_ERROR:   db "Disk read error!", 0
MSG_SECTORS_ERROR: db "Sector count error!", 0

; ============================================
; Boot sector padding and signature
; ============================================
times 510-($-$$) db 0
dw 0xAA55
