/*
 * NightOS - Programmable Interval Timer (PIT)
 * 
 * System timer for scheduling and time tracking
 */

#ifndef TIMER_H
#define TIMER_H

#include "types.h"

/* PIT I/O Ports */
#define PIT_CHANNEL0    0x40
#define PIT_CHANNEL1    0x41
#define PIT_CHANNEL2    0x42
#define PIT_COMMAND     0x43

/* PIT frequency (1.193182 MHz) */
#define PIT_FREQUENCY   1193182

/* Default timer frequency (100 Hz = 10ms per tick) */
#define TIMER_FREQUENCY 100

/* Timer functions */
void timer_init(uint32_t frequency);
void timer_wait(uint32_t ticks);
uint32_t timer_get_ticks(void);
uint32_t timer_get_seconds(void);
uint32_t timer_get_uptime_ms(void);

/* Sleep functions */
void sleep(uint32_t seconds);
void msleep(uint32_t milliseconds);

#endif /* TIMER_H */
