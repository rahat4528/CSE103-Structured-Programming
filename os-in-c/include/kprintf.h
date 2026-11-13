/**
 * CSE103-OS: Formatted Kernel Printf Routine
 * Writes formatted output simultaneously to VGA console and COM1 serial port.
 */

#ifndef CSE103_KPRINTF_H
#define CSE103_KPRINTF_H

#include "types.h"

int kprintf(const char *fmt, ...);
int ksnprintf(char *buf, size_t size, const char *fmt, ...);

#endif /* CSE103_KPRINTF_H */
