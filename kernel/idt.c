/*
 * NightOS - Interrupt Descriptor Table Implementation
 * 
 * Sets up CPU exception handlers and hardware interrupt handlers
 */

#include "../include/idt.h"
#include "../include/pic.h"
#include "../include/vga.h"
#include "../include/io.h"
#include "../include/string.h"

/* IDT and pointer */
static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t idt_ptr;

/* Interrupt handlers array */
static isr_handler_t interrupt_handlers[IDT_ENTRIES];

/* Exception messages */
const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 FPU Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point",
    "Virtualization",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved"
};

/* Set an IDT gate */
void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].offset_high = (handler >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].type_attr = flags;
}

/* Register a high-level interrupt handler */
void register_interrupt_handler(uint8_t n, isr_handler_t handler) {
    interrupt_handlers[n] = handler;
}

/* Initialize the IDT */
void idt_init(void) {
    /* Clear handlers */
    memset(interrupt_handlers, 0, sizeof(interrupt_handlers));
    
    /* Setup IDT pointer */
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint32_t)&idt;
    
    /* Clear IDT */
    memset(&idt, 0, sizeof(idt));
    
    /* Remap PIC: IRQ 0-7 to INT 32-39, IRQ 8-15 to INT 40-47 */
    pic_remap(0x20, 0x28);
    
    /* Install CPU exception handlers (ISR 0-31) */
    idt_set_gate(0, (uint32_t)isr0, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(1, (uint32_t)isr1, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(2, (uint32_t)isr2, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(3, (uint32_t)isr3, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(4, (uint32_t)isr4, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(5, (uint32_t)isr5, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(6, (uint32_t)isr6, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(7, (uint32_t)isr7, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(8, (uint32_t)isr8, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(9, (uint32_t)isr9, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(10, (uint32_t)isr10, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(11, (uint32_t)isr11, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(12, (uint32_t)isr12, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(13, (uint32_t)isr13, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(14, (uint32_t)isr14, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(15, (uint32_t)isr15, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(16, (uint32_t)isr16, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(17, (uint32_t)isr17, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(18, (uint32_t)isr18, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(19, (uint32_t)isr19, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(20, (uint32_t)isr20, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(21, (uint32_t)isr21, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(22, (uint32_t)isr22, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(23, (uint32_t)isr23, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(24, (uint32_t)isr24, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(25, (uint32_t)isr25, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(26, (uint32_t)isr26, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(27, (uint32_t)isr27, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(28, (uint32_t)isr28, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(29, (uint32_t)isr29, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(30, (uint32_t)isr30, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(31, (uint32_t)isr31, 0x08, IDT_GATE_INTERRUPT);
    
    /* Install hardware IRQ handlers (IRQ 0-15 -> INT 32-47) */
    idt_set_gate(32, (uint32_t)irq0, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(33, (uint32_t)irq1, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(34, (uint32_t)irq2, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(35, (uint32_t)irq3, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(36, (uint32_t)irq4, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(37, (uint32_t)irq5, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(38, (uint32_t)irq6, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(39, (uint32_t)irq7, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(40, (uint32_t)irq8, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(41, (uint32_t)irq9, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(42, (uint32_t)irq10, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(43, (uint32_t)irq11, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(44, (uint32_t)irq12, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(45, (uint32_t)irq13, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(46, (uint32_t)irq14, 0x08, IDT_GATE_INTERRUPT);
    idt_set_gate(47, (uint32_t)irq15, 0x08, IDT_GATE_INTERRUPT);
    
    /* Load IDT */
    idt_load((uint32_t)&idt_ptr);
}

/* ISR handler - called from assembly */
void isr_handler(registers_t* regs) {
    /* Check for registered handler */
    if (interrupt_handlers[regs->int_no]) {
        interrupt_handlers[regs->int_no](regs);
        return;
    }
    
    /* Default exception handler - display error and halt */
    vga_set_color(vga_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
    vga_puts("\n  KERNEL PANIC  \n");
    vga_set_color(vga_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    
    if (regs->int_no < 32) {
        vga_printf("\n  Exception: %s\n", exception_messages[regs->int_no]);
    } else {
        vga_printf("\n  Unknown interrupt: %d\n", regs->int_no);
    }
    
    vga_set_color(vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    vga_printf("  Error code: 0x%x\n", regs->err_code);
    vga_printf("  EIP: 0x%x\n", regs->eip);
    vga_printf("  CS:  0x%x\n", regs->cs);
    vga_printf("  EFLAGS: 0x%x\n", regs->eflags);
    
    vga_puts("\n  System halted.\n");
    
    /* Halt the system */
    __asm__ volatile("cli; hlt");
}

/* IRQ handler - called from assembly */
void irq_handler(registers_t* regs) {
    /* Send EOI to PIC */
    pic_send_eoi(regs->int_no - 32);
    
    /* Call registered handler if exists */
    if (interrupt_handlers[regs->int_no]) {
        interrupt_handlers[regs->int_no](regs);
    }
}
