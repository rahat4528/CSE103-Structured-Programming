/**
 * @file shared_memory_read.c
 * @brief Demonstration of POSIX shared memory reading and cleanup in Linux.
 *
 * Concepts Demonstrated:
 *  - Opening an existing POSIX shared memory object
 *  - Verifying segment size via fstat() before mapping to avoid SIGBUS
 *  - Mapping shared segment into reader's address space via mmap()
 *  - Extracting shared structured payload safely
 *  - Unmapping memory using munmap() and unlinking object with shm_unlink()
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define SHM_NAME "/os_c_shm_demo"

typedef struct {
    int sender_pid;
    int message_id;
    char text[128];
} SharedData;

int main(void) {
    printf("====================================================\n");
    printf("        POSIX Shared Memory Reader: mmap()          \n");
    printf("====================================================\n\n");

    const size_t shm_size = sizeof(SharedData);
    printf("[1] Opening existing shared memory object: \"%s\"...\n", SHM_NAME);

    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("[Error] shm_open failed");
        fprintf(stderr, "Hint: Did you run 'shared_memory_write' first?\n");
        return EXIT_FAILURE;
    }
    printf("    Shared memory object located. Descriptor: %d\n\n", shm_fd);

    // Verify segment size before mapping to prevent SIGBUS on empty/truncated objects
    struct stat shm_stat;
    if (fstat(shm_fd, &shm_stat) == -1) {
        perror("[Error] fstat failed");
        close(shm_fd);
        return EXIT_FAILURE;
    }
    if ((size_t)shm_stat.st_size < shm_size) {
        fprintf(stderr, "[Error] Shared memory object size (%lld bytes) is smaller than expected (%zu bytes).\n",
                (long long)shm_stat.st_size, shm_size);
        close(shm_fd);
        return EXIT_FAILURE;
    }

    printf("[2] Mapping shared memory segment as read-only via mmap()...\n");
    void *ptr = mmap(NULL, shm_size, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("[Error] mmap failed");
        close(shm_fd);
        return EXIT_FAILURE;
    }
    printf("    Mapped at virtual address: %p\n\n", ptr);

    printf("[3] Reading payload from shared memory:\n");
    const SharedData *data = (const SharedData *)ptr;
    printf("---------------- SHARED MEMORY DATA ----------------\n");
    printf("  Original Sender PID : %d\n", data->sender_pid);
    printf("  Message Sequence ID : %d\n", data->message_id);
    printf("  Transmitted Text    : \"%.*s\"\n", (int)sizeof(data->text) - 1, data->text);
    printf("----------------------------------------------------\n\n");

    printf("[4] Unmapping memory with munmap()...\n");
    if (munmap(ptr, shm_size) == -1) {
        perror("[Error] munmap failed");
        close(shm_fd);
        return EXIT_FAILURE;
    }

    if (close(shm_fd) == -1) {
        perror("[Error] close failed");
        return EXIT_FAILURE;
    }

    printf("[5] Cleaning up shared memory object with shm_unlink()...\n");
    if (shm_unlink(SHM_NAME) == -1) {
        perror("[Error] shm_unlink failed");
        return EXIT_FAILURE;
    }

    printf("    Shared memory object \"%s\" removed from /dev/shm.\n", SHM_NAME);
    printf("    No stale shared memory remains in the operating system.\n");
    printf("====================================================\n");

    return EXIT_SUCCESS;
}
