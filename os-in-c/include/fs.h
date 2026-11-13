/**
 * CSE103-OS: In-Memory RAM-Disk File System (ramfs)
 * Flat file system providing embedded course notes, viva sheets, and read/write files.
 */

#ifndef CSE103_FS_H
#define CSE103_FS_H

#include "types.h"

#define FS_MAX_FILES 32
#define FS_MAX_FILENAME 32
#define FS_MAX_FILESIZE 2048

typedef struct {
    char name[FS_MAX_FILENAME];
    size_t size;
    bool read_only;
    bool in_use;
    char data[FS_MAX_FILESIZE];
} fs_file_t;

void fs_init(void);
int fs_create(const char *name, const char *content, size_t size, bool read_only);
const char *fs_read(const char *name, size_t *out_size);
void fs_list(void);

#endif /* CSE103_FS_H */
