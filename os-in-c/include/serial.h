/**
 * CSE103-OS: COM1 Serial UART 16550 Driver
 * Handles serial initialization, polled and interrupt I/O, and boot logging on COM1.
 */

#ifndef CSE103_SERIAL_H
#define CSE103_SERIAL_H

#include "types.h"

#define COM1_PORT 0x3F8

void serial_init(void);
void serial_putc(char c);
void serial_puts(const char *str);
bool serial_received(void);
char serial_getc(void);

#endif /* CSE103_SERIAL_H */
