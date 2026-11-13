/**
 * CSE103-OS: Programmable Interrupt Controller (8259 PIC) Management
 * Remaps hardware IRQs away from CPU exception vectors (0-31) to 32-47.
 */

#ifndef CSE103_PIC_H
#define CSE103_PIC_H

#include "types.h"

#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1

#define PIC_EOI   0x20

#define PIC1_OFFSET 0x20
#define PIC2_OFFSET 0x28

void pic_init(void);
void pic_send_eoi(uint8_t irq);
void pic_set_mask(uint8_t irq_line);
void pic_clear_mask(uint8_t irq_line);

#endif /* CSE103_PIC_H */
