/**
 * CSE103-OS: Programmable Interrupt Controller (8259 PIC) Management
 * Remaps hardware IRQ vectors 0-15 from legacy BIOS vectors (0x08-0x0F)
 * to 0x20-0x2F (vectors 32-47) to avoid collisions with x86 CPU exceptions.
 */

#include "pic.h"
#include "io.h"

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_CMD, PIC_EOI);
    }
    outb(PIC1_CMD, PIC_EOI);
}

void pic_set_mask(uint8_t irq_line) {
    uint16_t port;
    uint8_t value;

    if (irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    value = (uint8_t)(inb(port) | (1 << irq_line));
    outb(port, value);
}

void pic_clear_mask(uint8_t irq_line) {
    uint16_t port;
    uint8_t value;

    if (irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    value = (uint8_t)(inb(port) & ~(1 << irq_line));
    outb(port, value);
}

void pic_init(void) {
    /*
     * Initialization Control Word 1 (ICW1): Start PIC initialization sequence
     * in cascade mode with ICW4 needed.
     */
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();

    /* ICW2: Remap Master PIC vector offset to 0x20 (32) */
    outb(PIC1_DATA, PIC1_OFFSET);
    io_wait();
    /* ICW2: Remap Slave PIC vector offset to 0x28 (40) */
    outb(PIC2_DATA, PIC2_OFFSET);
    io_wait();

    /* ICW3: Master has slave attached to IRQ2 (bit mask 00000100b = 0x04) */
    outb(PIC1_DATA, 0x04);
    io_wait();
    /* ICW3: Slave has cascade identity 2 */
    outb(PIC2_DATA, 0x02);
    io_wait();

    /* ICW4: Set 8086/88 architecture mode */
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    /* Mask all interrupts initially except IRQ2 cascade */
    outb(PIC1_DATA, 0xFB); /* 11111011b (only IRQ2 cascade unmasked) */
    outb(PIC2_DATA, 0xFF); /* Mask all slave IRQs */
}
