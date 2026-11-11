/**
 * @file basic_pipe.c
 * @brief Demonstration of unidirectional IPC using the POSIX pipe() system call.
 *
 * Concepts Demonstrated:
 *  - Creation of a kernel pipe buffer via pipe()
 *  - Understanding file descriptor pairs: pipefd[0] (read) and pipefd[1] (write)
 *  - Writing bytes to a pipe descriptor
 *  - Reading bytes from a pipe descriptor
 *  - Properly closing file descriptors
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    printf("====================================================\n");
    printf("       Inter-Process Communication: basic_pipe()    \n");
    printf("====================================================\n\n");

    // Array to hold the two file descriptors
    int pipefd[2];

    printf("[1] Creating kernel pipe with pipe()...\n");
    if (pipe(pipefd) == -1) {
        perror("pipe creation failed");
        return EXIT_FAILURE;
    }

    printf("    Pipe created successfully:\n");
    printf("    pipefd[0] (Read End File Descriptor)  : %d\n", pipefd[0]);
    printf("    pipefd[1] (Write End File Descriptor) : %d\n\n", pipefd[1]);

    const char *outgoing_message = "Operating Systems: Pipeline IPC in Action!";
    size_t msg_len = strlen(outgoing_message) + 1; // Include null terminator

    printf("[2] Writing message to pipefd[1]...\n");
    printf("    Outgoing Content : \"%s\" (%zu bytes)\n", outgoing_message, msg_len);

    ssize_t bytes_written = write(pipefd[1], outgoing_message, msg_len);
    if (bytes_written == -1 || (size_t)bytes_written != msg_len) {
        perror("write to pipe failed or incomplete");
        close(pipefd[0]);
        close(pipefd[1]);
        return EXIT_FAILURE;
    }
    printf("    Bytes written    : %zd\n\n", bytes_written);

    // Close write end before reading or after write is complete
    printf("[3] Closing write descriptor pipefd[1]...\n");
    if (close(pipefd[1]) == -1) {
        perror("close write end failed");
        close(pipefd[0]);
        return EXIT_FAILURE;
    }

    // Read from the pipe buffer
    char incoming_buffer[128];
    memset(incoming_buffer, 0, sizeof(incoming_buffer));

    printf("[4] Reading data from pipefd[0]...\n");
    ssize_t bytes_read = read(pipefd[0], incoming_buffer, sizeof(incoming_buffer) - 1);
    if (bytes_read <= 0) {
        if (bytes_read == -1) {
            perror("read from pipe failed");
        } else {
            fprintf(stderr, "Unexpected EOF while reading from pipe.\n");
        }
        close(pipefd[0]);
        return EXIT_FAILURE;
    }

    printf("    Bytes read       : %zd\n", bytes_read);
    printf("    Received Content : \"%s\"\n\n", incoming_buffer);

    // Close read end
    printf("[5] Closing read descriptor pipefd[0]...\n");
    if (close(pipefd[0]) == -1) {
        perror("close read end failed");
        return EXIT_FAILURE;
    }

    printf("====================================================\n");
    printf("  Pipe communication completed and cleaned up.\n");
    printf("====================================================\n");

    return EXIT_SUCCESS;
}
