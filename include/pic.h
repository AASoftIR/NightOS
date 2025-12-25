/*
 * NightOS - Programmable Interrupt Controller (PIC)
 * 
 * 8259 PIC driver for hardware interrupt management
 */

#ifndef PIC_H
#define PIC_H

#include "types.h"

/* PIC I/O ports */
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

/* PIC commands */
#define PIC_EOI      0x20   /* End of Interrupt */

/* ICW1 (Initialization Command Word 1) */
#define ICW1_ICW4      0x01 /* ICW4 needed */
#define ICW1_SINGLE    0x02 /* Single mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 */
#define ICW1_LEVEL     0x08 /* Level triggered mode */
#define ICW1_INIT      0x10 /* Initialization */

/* ICW4 (Initialization Command Word 4) */
#define ICW4_8086      0x01 /* 8086/88 mode */
#define ICW4_AUTO      0x02 /* Auto EOI */
#define ICW4_BUF_SLAVE 0x08 /* Buffered mode (slave) */
#define ICW4_BUF_MAST  0x0C /* Buffered mode (master) */
#define ICW4_SFNM      0x10 /* Special fully nested */

/* Initialize the PIC */
void pic_init(void);

/* Remap PIC IRQs to specified offsets */
void pic_remap(uint8_t offset1, uint8_t offset2);

/* Send End of Interrupt signal */
void pic_send_eoi(uint8_t irq);

/* Enable (unmask) an IRQ */
void pic_enable_irq(uint8_t irq);

/* Disable (mask) an IRQ */
void pic_disable_irq(uint8_t irq);

/* Get the current IRQ mask */
uint16_t pic_get_mask(void);

/* Set the IRQ mask */
void pic_set_mask(uint16_t mask);

#endif /* PIC_H */
