/**
 * CSE103-OS: In-Memory RAM-Disk File System (ramfs)
 * Simple flat file system storing kernel documentation, lab viva sheets,
 * and user-created files directly in RAM.
 */

#include "fs.h"
#include "string.h"
#include "kprintf.h"

static fs_file_t fs_files[FS_MAX_FILES];

void fs_init(void) {
    memset(fs_files, 0, sizeof(fs_files));

    /* Pre-populate embedded course and OS files */
    fs_create("/readme.txt",
              "Welcome to CSE103-OS!\n"
              "A 32-bit x86 protected-mode operating system written purely in C11.\n"
              "Features: GDT, IDT, PIC, PIT, PS/2 Keyboard, Serial COM1, PMM, Paging, Heap, Shell, Tasking.\n",
              0, true);

    fs_create("/viva.txt",
              "CSE103 Operating Systems Viva Study Guide:\n"
              "1. Process creation: fork(), exec(), waitpid().\n"
              "2. Synchronization: Mutexes, semaphores, race conditions, deadlock conditions.\n"
              "3. Memory: Paging, Page tables, CR3, Page fault (ISR 14), External vs Internal fragmentation.\n"
              "4. Scheduling: FCFS, SJF, Round-Robin, Priority scheduling.\n",
              0, true);

    fs_create("/system.info",
              "Kernel: CSE103-OS (i686)\n"
              "Mode: 32-bit Protected Mode\n"
              "Bootloader: Multiboot 1 / GNU GRUB\n"
              "Compiler: Freestanding C11\n"
              "Architecture: x86 PC-AT Compatible\n",
              0, true);
}

int fs_create(const char *name, const char *content, size_t size, bool read_only) {
    if (!name || !content) return -1;
    if (size == 0) size = strlen(content);
    if (size >= FS_MAX_FILESIZE) size = FS_MAX_FILESIZE - 1;

    /* Check if file already exists */
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_files[i].in_use && strcmp(fs_files[i].name, name) == 0) {
            if (fs_files[i].read_only) return -2; /* Read only */
            memcpy(fs_files[i].data, content, size);
            fs_files[i].data[size] = '\0';
            fs_files[i].size = size;
            return 0;
        }
    }

    /* Find empty slot */
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (!fs_files[i].in_use) {
            strncpy(fs_files[i].name, name, FS_MAX_FILENAME - 1);
            fs_files[i].name[FS_MAX_FILENAME - 1] = '\0';
            memcpy(fs_files[i].data, content, size);
            fs_files[i].data[size] = '\0';
            fs_files[i].size = size;
            fs_files[i].read_only = read_only;
            fs_files[i].in_use = true;
            return 0;
        }
    }

    return -3; /* RAM disk full */
}

const char *fs_read(const char *name, size_t *out_size) {
    if (!name) return NULL;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_files[i].in_use && strcmp(fs_files[i].name, name) == 0) {
            if (out_size) *out_size = fs_files[i].size;
            return fs_files[i].data;
        }
    }
    return NULL;
}

void fs_list(void) {
    kprintf("Type  Size (bytes)  Filename\n");
    kprintf("----  ------------  --------\n");
    int count = 0;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_files[i].in_use) {
            kprintf("%s  %d           %s\n",
                    fs_files[i].read_only ? "[RO]" : "[RW]",
                    (int)fs_files[i].size,
                    fs_files[i].name);
            count++;
        }
    }
    kprintf("Total files: %d\n", count);
}
