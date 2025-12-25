/*
 * NightOS - PIT Timer Implementation
 * 
 * System timer using the 8253/8254 PIT
 */

#include "../include/timer.h"
#include "../include/idt.h"
#include "../include/io.h"
#include "../include/vga.h"

/* Global tick counter */
static volatile uint32_t timer_ticks = 0;
static uint32_t timer_freq = TIMER_FREQUENCY;

/* Timer interrupt handler */
static void timer_callback(registers_t* regs) {
    UNUSED(regs);
    timer_ticks++;
}

/* Initialize the PIT */
void timer_init(uint32_t frequency) {
    timer_freq = frequency;
    
    /* Calculate divisor */
    uint32_t divisor = PIT_FREQUENCY / frequency;
    
    /* Send command byte: Channel 0, lobyte/hibyte, rate generator */
    outb(PIT_COMMAND, 0x36);
    
    /* Send divisor */
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));         /* Low byte */
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));  /* High byte */
    
    /* Register timer handler on IRQ0 */
    register_interrupt_handler(IRQ0, timer_callback);
}

/* Get current tick count */
uint32_t timer_get_ticks(void) {
    return timer_ticks;
}

/* Get uptime in seconds */
uint32_t timer_get_seconds(void) {
    return timer_ticks / timer_freq;
}

/* Get uptime in milliseconds */
uint32_t timer_get_uptime_ms(void) {
    return (timer_ticks * 1000) / timer_freq;
}

/* Wait for specified number of ticks */
void timer_wait(uint32_t ticks) {
    uint32_t target = timer_ticks + ticks;
    while (timer_ticks < target) {
        __asm__ volatile("hlt");  /* Halt until next interrupt */
    }
}

/* Sleep for specified seconds */
void sleep(uint32_t seconds) {
    timer_wait(seconds * timer_freq);
}

/* Sleep for specified milliseconds */
void msleep(uint32_t milliseconds) {
    timer_wait((milliseconds * timer_freq) / 1000);
}
