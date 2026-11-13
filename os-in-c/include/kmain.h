/**
 * CSE103-OS: Kernel Main Entry Header
 */

#ifndef CSE103_KMAIN_H
#define CSE103_KMAIN_H

#include "types.h"
#include "multiboot.h"

void kmain(uint32_t magic, multiboot_info_t *mbi) __attribute__((noreturn));

#endif /* CSE103_KMAIN_H */
