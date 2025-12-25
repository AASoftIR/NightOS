/*
 * NightOS - Programmable Interrupt Controller Implementation
 * 
 * 8259 PIC driver for managing hardware interrupts
 */

#include "../include/pic.h"
#include "../include/io.h"

/* Remap PIC IRQs to specified offsets */
void pic_remap(uint8_t offset1, uint8_t offset2) {
    uint8_t mask1, mask2;
    
    /* Save current masks */
    mask1 = inb(PIC1_DATA);
    mask2 = inb(PIC2_DATA);
    
    /* Start initialization sequence (cascade mode) */
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    
    /* Set vector offsets */
    outb(PIC1_DATA, offset1);  /* Master PIC vector offset */
    io_wait();
    outb(PIC2_DATA, offset2);  /* Slave PIC vector offset */
    io_wait();
    
    /* Tell Master PIC about Slave PIC at IRQ2 */
    outb(PIC1_DATA, 0x04);
    io_wait();
    /* Tell Slave PIC its cascade identity */
    outb(PIC2_DATA, 0x02);
    io_wait();
    
    /* Set 8086 mode */
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();
    
    /* Restore saved masks */
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

/* Initialize PIC */
void pic_init(void) {
    /* Remap IRQs 0-7 to INT 32-39, IRQs 8-15 to INT 40-47 */
    pic_remap(0x20, 0x28);
    
    /* Enable all IRQs (unmask all) */
    outb(PIC1_DATA, 0x00);
    outb(PIC2_DATA, 0x00);
}

/* Send End of Interrupt signal */
void pic_send_eoi(uint8_t irq) {
    /* If IRQ came from slave PIC, send EOI to slave */
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    /* Always send EOI to master */
    outb(PIC1_COMMAND, PIC_EOI);
}

/* Enable (unmask) an IRQ */
void pic_enable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

/* Disable (mask) an IRQ */
void pic_disable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    
    value = inb(port) | (1 << irq);
    outb(port, value);
}

/* Get current IRQ mask */
uint16_t pic_get_mask(void) {
    return inb(PIC1_DATA) | (inb(PIC2_DATA) << 8);
}

/* Set IRQ mask */
void pic_set_mask(uint16_t mask) {
    outb(PIC1_DATA, mask & 0xFF);
    outb(PIC2_DATA, (mask >> 8) & 0xFF);
}
