/*
 * NightOS - Interrupt Descriptor Table (IDT)
 * 
 * Handles CPU exceptions and hardware interrupts
 */

#ifndef IDT_H
#define IDT_H

#include "types.h"

/* Number of IDT entries */
#define IDT_ENTRIES 256

/* IDT gate types */
#define IDT_GATE_INTERRUPT 0x8E  /* 32-bit Interrupt Gate */
#define IDT_GATE_TRAP      0x8F  /* 32-bit Trap Gate */

/* IDT entry structure (8 bytes) */
typedef struct {
    uint16_t offset_low;    /* Lower 16 bits of handler address */
    uint16_t selector;      /* Kernel code segment selector */
    uint8_t  zero;          /* Always zero */
    uint8_t  type_attr;     /* Type and attributes */
    uint16_t offset_high;   /* Upper 16 bits of handler address */
} __attribute__((packed)) idt_entry_t;

/* IDT pointer structure */
typedef struct {
    uint16_t limit;         /* Size of IDT - 1 */
    uint32_t base;          /* Base address of IDT */
} __attribute__((packed)) idt_ptr_t;

/* Interrupt handler function type */
typedef void (*interrupt_handler_t)(void);

/* CPU Exception names */
extern const char* exception_messages[];

/* Initialize IDT */
void idt_init(void);

/* Set an IDT entry */
void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags);

/* Register an interrupt handler */
void idt_register_handler(uint8_t num, interrupt_handler_t handler);

/* Load IDT into CPU */
extern void idt_load(uint32_t idt_ptr);

/* ISR declarations (defined in assembly) */
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

/* IRQ declarations (defined in assembly) */
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

/* IRQ numbers */
#define IRQ0  32
#define IRQ1  33
#define IRQ2  34
#define IRQ3  35
#define IRQ4  36
#define IRQ5  37
#define IRQ6  38
#define IRQ7  39
#define IRQ8  40
#define IRQ9  41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

/* Registers structure pushed by ISR */
typedef struct {
    uint32_t ds;                                     /* Data segment */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Pushed by pusha */
    uint32_t int_no, err_code;                       /* Interrupt number and error code */
    uint32_t eip, cs, eflags, useresp, ss;           /* Pushed by CPU */
} __attribute__((packed)) registers_t;

/* High-level interrupt handler type */
typedef void (*isr_handler_t)(registers_t*);

/* Register a high-level interrupt handler */
void register_interrupt_handler(uint8_t n, isr_handler_t handler);

#endif /* IDT_H */
