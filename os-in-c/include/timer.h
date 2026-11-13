/**
 * CSE103-OS: Programmable Interval Timer (PIT 8253/8254) Driver
 * Drives periodic system clock ticks and precise sleep delays.
 */

#ifndef CSE103_TIMER_H
#define CSE103_TIMER_H

#include "types.h"

#define PIT_DEFAULT_HZ 100

void timer_init(uint32_t frequency);
void timer_sleep_ticks(uint32_t ticks);
uint32_t timer_get_ticks(void);
uint32_t timer_get_uptime_sec(void);

#endif /* CSE103_TIMER_H */
