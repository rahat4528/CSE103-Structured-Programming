/**
 * CSE103-OS: Multiboot 1 Specification Definitions
 * Defines Multiboot magic headers, information flags, and structures passed by GRUB.
 */

#ifndef CSE103_MULTIBOOT_H
#define CSE103_MULTIBOOT_H

#include "types.h"

#define MULTIBOOT_HEADER_MAGIC          0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002

#define MULTIBOOT_PAGE_ALIGN            (1 << 0)
#define MULTIBOOT_MEMORY_INFO           (1 << 1)

#define MULTIBOOT_INFO_MEMORY           (1 << 0)
#define MULTIBOOT_INFO_BOOTDEV          (1 << 1)
#define MULTIBOOT_INFO_CMDLINE          (1 << 2)
#define MULTIBOOT_INFO_MODS             (1 << 3)
#define MULTIBOOT_INFO_AOUT_SYMS        (1 << 4)
#define MULTIBOOT_INFO_ELF_SHDR         (1 << 5)
#define MULTIBOOT_INFO_MEM_MAP          (1 << 6)

#define MULTIBOOT_MEMORY_AVAILABLE      1
#define MULTIBOOT_MEMORY_RESERVED       2
#define MULTIBOOT_MEMORY_ACPI_RECLAIM   3
#define MULTIBOOT_MEMORY_NVS            4
#define MULTIBOOT_MEMORY_BADRAM         5

struct multiboot_header {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
    uint32_t mode_type;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
};

typedef struct multiboot_mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed)) multiboot_mmap_entry_t;

typedef struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
} __attribute__((packed)) multiboot_info_t;

#endif /* CSE103_MULTIBOOT_H */
