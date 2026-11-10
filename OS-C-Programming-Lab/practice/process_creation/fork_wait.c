/**
 * @file fork_wait.c
 * @brief Demonstration of process synchronization using fork() and wait().
 *
 * Concepts Demonstrated:
 *  - Parent waiting for child termination with wait()
 *  - Inspecting child exit status via WIFEXITED and WEXITSTATUS
 *  - Preventing zombie processes by reaping the termination status
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    printf("====================================================\n");
    printf("      Process Synchronization: fork() & wait()      \n");
    printf("====================================================\n\n");

    printf("[Parent] PID %d is creating a child process...\n", (int)getpid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // Child Process
        printf("  [Child] PID %d started. Performing simulated task...\n", (int)getpid());
        for (int i = 1; i <= 3; i++) {
            printf("  [Child] Step %d of 3 completed...\n", i);
            sleep(1);
        }

        const int exit_code = 42;
        printf("  [Child] Terminating voluntarily with exit code %d.\n", exit_code);
        exit(exit_code);
    } else {
        // Parent Process
        printf("[Parent] Waiting for child PID %d to terminate...\n", (int)pid);

        int status = 0;
        pid_t reaped_pid = wait(&status);

        if (reaped_pid < 0) {
            perror("wait failed");
            return EXIT_FAILURE;
        }

        printf("\n[Parent] Child process %d has terminated.\n", (int)reaped_pid);

        // Check if the child exited normally
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("[Parent] Termination: Normal exit.\n");
            printf("[Parent] Reaped Exit Code : %d\n", exit_code);
            if (exit_code == 42) {
                printf("[Parent] Verification successful: Exit code matches child's sent status.\n");
            }
        } else if (WIFSIGNALED(status)) {
            printf("[Parent] Child terminated abnormally by signal: %d\n", WTERMSIG(status));
        }

        printf("[Parent] Zombie prevention complete. Parent exiting cleanly.\n");
    }

    return EXIT_SUCCESS;
}
