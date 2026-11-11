/**
 * @file parent_child_pipe.c
 * @brief Inter-process communication between parent and child via a UNIX pipe.
 *
 * Concepts Demonstrated:
 *  - Creating pipe before fork() so descriptors are inherited by child
 *  - Closing unused ends in each process (critical to avoid deadlocks)
 *  - Unidirectional stream transmission across process boundaries
 *  - Waiting for child exit and clean descriptor reclamation
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    printf("====================================================\n");
    printf("   Parent-Child Pipe IPC: parent_child_pipe.c       \n");
    printf("====================================================\n\n");

    int pipefd[2];

    // Create the pipe BEFORE fork so child inherits both descriptors
    if (pipe(pipefd) == -1) {
        perror("pipe creation failed");
        return EXIT_FAILURE;
    }

    printf("[Parent (PID %d)] Pipe initialized: read_fd=%d, write_fd=%d\n",
           (int)getpid(), pipefd[0], pipefd[1]);
    printf("[Parent] Forking child process...\n\n");
    fflush(stdout);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        close(pipefd[0]);
        close(pipefd[1]);
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // ----------------------------------------------------
        // CHILD PROCESS: READER
        // ----------------------------------------------------
        // Rule: Close the unused end immediately!
        // The child only reads, so it must close the write end (pipefd[1]).
        close(pipefd[1]);

        printf("  [Child (PID %d)] Closed unused write descriptor %d.\n",
               (int)getpid(), pipefd[1]);
        printf("  [Child] Waiting to receive message from parent...\n");

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));

        ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            if (bytes_read == -1) {
                perror("child read failed");
            } else {
                fprintf(stderr, "  [Child] Parent closed pipe without sending data.\n");
            }
            close(pipefd[0]);
            exit(EXIT_FAILURE);
        }

        printf("  [Child] Successfully read %zd bytes!\n", bytes_read);
        printf("  [Child] Message Content: \"%s\"\n", buffer);

        // Close reading descriptor when done
        close(pipefd[0]);
        printf("  [Child] Closed read descriptor. Exiting.\n");
        fflush(stdout);
        exit(EXIT_SUCCESS);
    } else {
        // ----------------------------------------------------
        // PARENT PROCESS: WRITER
        // ----------------------------------------------------
        // Rule: Close the unused end immediately!
        // The parent only writes, so it must close the read end (pipefd[0]).
        close(pipefd[0]);

        printf("[Parent] Closed unused read descriptor %d.\n", pipefd[0]);

        const char *message = "Hello from Parent! Remember to close unused pipe descriptors.";
        size_t msg_len = strlen(message) + 1;
        printf("[Parent] Transmitting payload to child: \"%s\"\n", message);

        // Sleep briefly to show synchronization
        sleep(1);

        ssize_t bytes_written = write(pipefd[1], message, msg_len);
        if (bytes_written == -1 || (size_t)bytes_written != msg_len) {
            perror("parent write failed");
            close(pipefd[1]);
            waitpid(pid, NULL, 0);
            return EXIT_FAILURE;
        }

        printf("[Parent] Wrote %zd bytes. Closing write descriptor %d (sends EOF)...\n",
               bytes_written, pipefd[1]);
        close(pipefd[1]);

        // Wait for child to finish execution
        int status = 0;
        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid failed");
            return EXIT_FAILURE;
        }

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "[Parent] Child terminated abnormally or with failure status.\n");
            return EXIT_FAILURE;
        }

        printf("[Parent] Child process reaped successfully. Done.\n");
    }

    return EXIT_SUCCESS;
}
