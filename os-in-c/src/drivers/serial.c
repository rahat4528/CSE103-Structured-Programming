/**
 * CSE103-OS: COM1 Serial UART 16550 Driver
 * Handles UART configuration at 115200 baud 8N1, line status polling,
 * character transmission, and reception for automated test harnesses.
 */

#include "serial.h"
#include "io.h"

#define UART_DATA(base)         (base)
#define UART_INT_ENABLE(base)   ((base) + 1)
#define UART_FIFO_CTRL(base)    ((base) + 2)
#define UART_LINE_CTRL(base)    ((base) + 3)
#define UART_MODEM_CTRL(base)   ((base) + 4)
#define UART_LINE_STATUS(base)  ((base) + 5)

void serial_init(void) {
    /* Disable all interrupts on COM1 during initialization */
    outb(UART_INT_ENABLE(COM1_PORT), 0x00);

    /* Enable DLAB (Divisor Latch Access Bit) to configure baud rate */
    outb(UART_LINE_CTRL(COM1_PORT), 0x80);

    /* Set divisor to 1 (115200 baud) for fast QEMU communication */
    outb(UART_DATA(COM1_PORT), 0x01);
    outb(UART_INT_ENABLE(COM1_PORT), 0x00);

    /* 8 bits, no parity, one stop bit (8N1), clear DLAB */
    outb(UART_LINE_CTRL(COM1_PORT), 0x03);

    /* Enable FIFO, clear TX/RX FIFOs, set 14-byte interrupt threshold */
    outb(UART_FIFO_CTRL(COM1_PORT), 0xC7);

    /* Configure Aux Output 2 (enables interrupts via PIC), RTS and DTR high */
    outb(UART_MODEM_CTRL(COM1_PORT), 0x0B);

    /* Enable Received Data Available interrupt */
    outb(UART_INT_ENABLE(COM1_PORT), 0x01);
}

static bool serial_is_transmit_empty(void) {
    return (inb(UART_LINE_STATUS(COM1_PORT)) & 0x20) != 0;
}

void serial_putc(char c) {
    if (c == '\n') {
        while (!serial_is_transmit_empty()) {
            /* Spin wait for transmit buffer empty */
        }
        outb(UART_DATA(COM1_PORT), '\r');
    }

    while (!serial_is_transmit_empty()) {
        /* Spin wait for transmit buffer empty */
    }
    outb(UART_DATA(COM1_PORT), (uint8_t)c);
}

void serial_puts(const char *str) {
    if (!str) return;
    while (*str) {
        serial_putc(*str++);
    }
}

bool serial_received(void) {
    return (inb(UART_LINE_STATUS(COM1_PORT)) & 0x01) != 0;
}

char serial_getc(void) {
    while (!serial_received()) {
        /* Spin wait for incoming character */
    }
    return (char)inb(UART_DATA(COM1_PORT));
}
