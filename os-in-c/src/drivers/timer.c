/**
 * CSE103-OS: Programmable Interval Timer (PIT 8253/8254) Driver
 * Drives system clock ticks at 100 Hz, provides sleep delay routines,
 * uptime tracking, and periodic scheduler preemption hooks.
 */

#include "timer.h"
#include "irq.h"
#include "io.h"
#include "context.h"

#define PIT_CHANNEL0_DATA 0x40
#define PIT_MODE_COMMAND  0x43
#define PIT_BASE_FREQUENCY 1193180

static volatile uint32_t system_ticks = 0;
static uint32_t timer_frequency = PIT_DEFAULT_HZ;

static void timer_callback(registers_t *regs) {
    (void)regs;
    system_ticks++;

    /* Hook into round-robin task scheduler on every timer tick */
    task_schedule();
}

void timer_init(uint32_t frequency) {
    if (frequency == 0) frequency = PIT_DEFAULT_HZ;
    timer_frequency = frequency;

    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;
    if (divisor > 65535) divisor = 65535;

    /*
     * Configure PIT Channel 0 in Mode 3 (Square Wave Generator),
     * access mode: low byte then high byte (0x36 = 00110110b).
     */
    outb(PIT_MODE_COMMAND, 0x36);
    outb(PIT_CHANNEL0_DATA, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0_DATA, (uint8_t)((divisor >> 8) & 0xFF));

    irq_register_handler(0, timer_callback);
}

void timer_sleep_ticks(uint32_t ticks) {
    uint32_t target = system_ticks + ticks;
    while (system_ticks < target) {
        hlt();
    }
}

uint32_t timer_get_ticks(void) {
    return system_ticks;
}

uint32_t timer_get_uptime_sec(void) {
    return system_ticks / timer_frequency;
}
