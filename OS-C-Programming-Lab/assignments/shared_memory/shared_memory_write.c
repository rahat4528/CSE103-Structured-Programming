/**
 * @file shared_memory_write.c
 * @brief Demonstration of POSIX shared memory writing in Linux (shm_open, ftruncate, mmap).
 *
 * Concepts Demonstrated:
 *  - Creating or opening a POSIX shared memory object using shm_open()
 *  - Setting segment size using ftruncate()
 *  - Mapping shared memory into the process address space with mmap()
 *  - Zeroing memory and writing structured inter-process communication data
 *  - Unmapping memory using munmap() and closing descriptors
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define SHM_NAME "/os_c_shm_demo"
#define DEFAULT_MESSAGE "Inter-Process Communication via Linux POSIX Shared Memory!"

typedef struct {
    int sender_pid;
    int message_id;
    char text[128];
} SharedData;

int main(int argc, char *argv[]) {
    printf("====================================================\n");
    printf("       POSIX Shared Memory Writer: shm_open()       \n");
    printf("====================================================\n\n");

    const char *payload_msg = DEFAULT_MESSAGE;
    if (argc > 1) {
        if (argv[1][0] == '\0') {
            fprintf(stderr, "[Error] Message argument cannot be empty.\n");
            return EXIT_FAILURE;
        }
        payload_msg = argv[1];
    }

    const size_t shm_size = sizeof(SharedData);
    printf("[1] Creating / opening shared memory object: \"%s\"...\n", SHM_NAME);

    // shm_open creates a file under /dev/shm in Linux
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("[Error] shm_open failed");
        return EXIT_FAILURE;
    }
    printf("    Shared memory object opened. Descriptor: %d\n\n", shm_fd);

    printf("[2] Setting shared memory size to %zu bytes with ftruncate()...\n", shm_size);
    if (ftruncate(shm_fd, (off_t)shm_size) == -1) {
        perror("[Error] ftruncate failed");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }
    printf("    Memory segment sized successfully.\n\n");

    printf("[3] Mapping memory segment into process address space with mmap()...\n");
    void *ptr = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("[Error] mmap failed");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }
    printf("    Memory mapped at virtual address: %p\n\n", ptr);

    printf("[4] Zero-initializing segment and writing structured data...\n");
    // Clear any stale bytes before writing fresh data
    memset(ptr, 0, shm_size);

    SharedData *data = (SharedData *)ptr;
    data->sender_pid = (int)getpid();
    data->message_id = 101;
    strncpy(data->text, payload_msg, sizeof(data->text) - 1);
    data->text[sizeof(data->text) - 1] = '\0';

    printf("    Sender PID : %d\n", data->sender_pid);
    printf("    Message ID : %d\n", data->message_id);
    printf("    Payload    : \"%s\"\n\n", data->text);

    printf("[5] Synchronizing and unmapping with munmap()...\n");
    if (munmap(ptr, shm_size) == -1) {
        perror("[Error] munmap failed");
        close(shm_fd);
        return EXIT_FAILURE;
    }

    // Close the file descriptor (shared memory persists in kernel)
    if (close(shm_fd) == -1) {
        perror("[Error] close failed");
        return EXIT_FAILURE;
    }

    printf("    File descriptor closed. Shared memory object remains resident in /dev/shm.\n");
    printf("\n>>> NEXT STEP: Run 'shared_memory_read' to inspect and unlink this object.\n");
    printf("====================================================\n");

    return EXIT_SUCCESS;
}
