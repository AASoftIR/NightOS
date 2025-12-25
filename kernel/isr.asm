; NightOS - Interrupt Service Routines (Assembly)
; Low-level interrupt handlers

[BITS 32]

; Handle MinGW naming convention (underscore prefix for C symbols)
%ifdef MINGW
; Export ISR handlers with underscore prefix
global _isr0, _isr1, _isr2, _isr3, _isr4, _isr5, _isr6, _isr7
global _isr8, _isr9, _isr10, _isr11, _isr12, _isr13, _isr14, _isr15
global _isr16, _isr17, _isr18, _isr19, _isr20, _isr21, _isr22, _isr23
global _isr24, _isr25, _isr26, _isr27, _isr28, _isr29, _isr30, _isr31
global _irq0, _irq1, _irq2, _irq3, _irq4, _irq5, _irq6, _irq7
global _irq8, _irq9, _irq10, _irq11, _irq12, _irq13, _irq14, _irq15
global _idt_load

extern _isr_handler
extern _irq_handler

; Define aliases
%define isr0 _isr0
%define isr1 _isr1
%define isr2 _isr2
%define isr3 _isr3
%define isr4 _isr4
%define isr5 _isr5
%define isr6 _isr6
%define isr7 _isr7
%define isr8 _isr8
%define isr9 _isr9
%define isr10 _isr10
%define isr11 _isr11
%define isr12 _isr12
%define isr13 _isr13
%define isr14 _isr14
%define isr15 _isr15
%define isr16 _isr16
%define isr17 _isr17
%define isr18 _isr18
%define isr19 _isr19
%define isr20 _isr20
%define isr21 _isr21
%define isr22 _isr22
%define isr23 _isr23
%define isr24 _isr24
%define isr25 _isr25
%define isr26 _isr26
%define isr27 _isr27
%define isr28 _isr28
%define isr29 _isr29
%define isr30 _isr30
%define isr31 _isr31
%define irq0 _irq0
%define irq1 _irq1
%define irq2 _irq2
%define irq3 _irq3
%define irq4 _irq4
%define irq5 _irq5
%define irq6 _irq6
%define irq7 _irq7
%define irq8 _irq8
%define irq9 _irq9
%define irq10 _irq10
%define irq11 _irq11
%define irq12 _irq12
%define irq13 _irq13
%define irq14 _irq14
%define irq15 _irq15
%define idt_load _idt_load
%define ISR_HANDLER _isr_handler
%define IRQ_HANDLER _irq_handler

%else
; Export ISR handlers without prefix (ELF format)
global isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7
global isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15
global isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
global isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
global irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7
global irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15
global idt_load

extern isr_handler
extern irq_handler
%define ISR_HANDLER isr_handler
%define IRQ_HANDLER irq_handler
%endif

; ============================================
; Load IDT
; ============================================
idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

; ============================================
; ISR Common Stub
; ============================================
isr_common_stub:
    ; Save all registers
    pusha
    
    ; Save data segment
    mov ax, ds
    push eax
    
    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Push pointer to registers structure
    push esp
    
    ; Call C handler
    call ISR_HANDLER
    
    ; Remove pushed parameter
    add esp, 4
    
    ; Restore data segment
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Restore registers
    popa
    
    ; Remove error code and ISR number
    add esp, 8
    
    ; Return from interrupt
    iret

; ============================================
; IRQ Common Stub
; ============================================
irq_common_stub:
    ; Save all registers
    pusha
    
    ; Save data segment
    mov ax, ds
    push eax
    
    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Push pointer to registers structure
    push esp
    
    ; Call C handler
    call IRQ_HANDLER
    
    ; Remove pushed parameter
    add esp, 4
    
    ; Restore data segment
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Restore registers
    popa
    
    ; Remove error code and IRQ number
    add esp, 8
    
    ; Return from interrupt
    iret

; ============================================
; CPU Exception ISRs (0-31)
; ============================================

; ISR 0: Division By Zero
isr0:
    push byte 0     ; Dummy error code
    push byte 0     ; Interrupt number
    jmp isr_common_stub

; ISR 1: Debug
isr1:
    push byte 0
    push byte 1
    jmp isr_common_stub

; ISR 2: Non Maskable Interrupt
isr2:
    push byte 0
    push byte 2
    jmp isr_common_stub

; ISR 3: Breakpoint
isr3:
    push byte 0
    push byte 3
    jmp isr_common_stub

; ISR 4: Overflow
isr4:
    push byte 0
    push byte 4
    jmp isr_common_stub

; ISR 5: Bound Range Exceeded
isr5:
    push byte 0
    push byte 5
    jmp isr_common_stub

; ISR 6: Invalid Opcode
isr6:
    push byte 0
    push byte 6
    jmp isr_common_stub

; ISR 7: Device Not Available
isr7:
    push byte 0
    push byte 7
    jmp isr_common_stub

; ISR 8: Double Fault (with error code)
isr8:
    push byte 8
    jmp isr_common_stub

; ISR 9: Coprocessor Segment Overrun
isr9:
    push byte 0
    push byte 9
    jmp isr_common_stub

; ISR 10: Invalid TSS (with error code)
isr10:
    push byte 10
    jmp isr_common_stub

; ISR 11: Segment Not Present (with error code)
isr11:
    push byte 11
    jmp isr_common_stub

; ISR 12: Stack-Segment Fault (with error code)
isr12:
    push byte 12
    jmp isr_common_stub

; ISR 13: General Protection Fault (with error code)
isr13:
    push byte 13
    jmp isr_common_stub

; ISR 14: Page Fault (with error code)
isr14:
    push byte 14
    jmp isr_common_stub

; ISR 15: Reserved
isr15:
    push byte 0
    push byte 15
    jmp isr_common_stub

; ISR 16: x87 FPU Error
isr16:
    push byte 0
    push byte 16
    jmp isr_common_stub

; ISR 17: Alignment Check (with error code)
isr17:
    push byte 17
    jmp isr_common_stub

; ISR 18: Machine Check
isr18:
    push byte 0
    push byte 18
    jmp isr_common_stub

; ISR 19: SIMD Floating-Point
isr19:
    push byte 0
    push byte 19
    jmp isr_common_stub

; ISR 20: Virtualization
isr20:
    push byte 0
    push byte 20
    jmp isr_common_stub

; ISR 21-31: Reserved
isr21:
    push byte 0
    push byte 21
    jmp isr_common_stub

isr22:
    push byte 0
    push byte 22
    jmp isr_common_stub

isr23:
    push byte 0
    push byte 23
    jmp isr_common_stub

isr24:
    push byte 0
    push byte 24
    jmp isr_common_stub

isr25:
    push byte 0
    push byte 25
    jmp isr_common_stub

isr26:
    push byte 0
    push byte 26
    jmp isr_common_stub

isr27:
    push byte 0
    push byte 27
    jmp isr_common_stub

isr28:
    push byte 0
    push byte 28
    jmp isr_common_stub

isr29:
    push byte 0
    push byte 29
    jmp isr_common_stub

isr30:
    push byte 0
    push byte 30
    jmp isr_common_stub

isr31:
    push byte 0
    push byte 31
    jmp isr_common_stub

; ============================================
; Hardware IRQs (32-47)
; ============================================

; IRQ 0: Timer
irq0:
    push byte 0
    push byte 32
    jmp irq_common_stub

; IRQ 1: Keyboard
irq1:
    push byte 1
    push byte 33
    jmp irq_common_stub

; IRQ 2: Cascade
irq2:
    push byte 2
    push byte 34
    jmp irq_common_stub

; IRQ 3: COM2
irq3:
    push byte 3
    push byte 35
    jmp irq_common_stub

; IRQ 4: COM1
irq4:
    push byte 4
    push byte 36
    jmp irq_common_stub

; IRQ 5: LPT2
irq5:
    push byte 5
    push byte 37
    jmp irq_common_stub

; IRQ 6: Floppy
irq6:
    push byte 6
    push byte 38
    jmp irq_common_stub

; IRQ 7: LPT1 / Spurious
irq7:
    push byte 7
    push byte 39
    jmp irq_common_stub

; IRQ 8: RTC
irq8:
    push byte 8
    push byte 40
    jmp irq_common_stub

; IRQ 9: Free
irq9:
    push byte 9
    push byte 41
    jmp irq_common_stub

; IRQ 10: Free
irq10:
    push byte 10
    push byte 42
    jmp irq_common_stub

; IRQ 11: Free
irq11:
    push byte 11
    push byte 43
    jmp irq_common_stub

; IRQ 12: PS/2 Mouse
irq12:
    push byte 12
    push byte 44
    jmp irq_common_stub

; IRQ 13: FPU
irq13:
    push byte 13
    push byte 45
    jmp irq_common_stub

; IRQ 14: Primary ATA
irq14:
    push byte 14
    push byte 46
    jmp irq_common_stub

; IRQ 15: Secondary ATA
irq15:
    push byte 15
    push byte 47
    jmp irq_common_stub
