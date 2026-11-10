/**
 * @file fork_example.c
 * @brief Demonstration of process creation and address space isolation using fork().
 *
 * Concepts Demonstrated:
 *  - Process duplication using fork()
 *  - Identifying processes via getpid() (Current Process ID) and getppid() (Parent Process ID)
 *  - Independent execution paths for parent and child processes
 *  - Memory isolation: child gets a private copy of the parent's address space
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    printf("====================================================\n");
    printf("       Process Creation Demonstration: fork()       \n");
    printf("====================================================\n\n");

    int shared_variable = 100;
    printf("[Initial State] Before fork():\n");
    printf("  Parent Process PID : %d\n", (int)getpid());
    printf("  shared_variable    : %d (Address: %p)\n\n", shared_variable, (void *)&shared_variable);

    printf("Calling fork()...\n");
    pid_t pid = fork();

    if (pid < 0) {
        // fork() returns a negative value on failure
        perror("fork failed");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // ----------------------------------------------------
        // CHILD PROCESS CONTEXT
        // ----------------------------------------------------
        // In the child, fork() returns 0.
        printf("\n>>> [Child Process Branch]\n");
        printf("  Returned PID from fork() : %d\n", (int)pid);
        printf("  Child PID (getpid)       : %d\n", (int)getpid());
        printf("  Parent PID (getppid)     : %d\n", (int)getppid());
        printf("  Child sees shared_variable = %d\n", shared_variable);

        // Modify variable in child to demonstrate memory isolation
        shared_variable += 50;
        printf("  Child modified shared_variable to: %d\n", shared_variable);
        printf("  Child variable address   : %p\n", (void *)&shared_variable);
        printf(">>> [Child Process Completed]\n");

        exit(EXIT_SUCCESS);
    } else {
        // ----------------------------------------------------
        // PARENT PROCESS CONTEXT
        // ----------------------------------------------------
        // In the parent, fork() returns the PID of the child.
        // Brief sleep to allow child to complete printing clearly
        sleep(1);

        printf("\n<<< [Parent Process Branch]\n");
        printf("  Returned PID from fork() : %d (Child's PID)\n", (int)pid);
        printf("  Parent PID (getpid)      : %d\n", (int)getpid());
        printf("  Grandparent PID (getppid): %d\n", (int)getppid());
        printf("  Parent sees shared_variable = %d\n", shared_variable);
        printf("  Notice: shared_variable remains 100 in the parent!\n");
        printf("  This proves the child received an isolated copy of memory.\n");
        printf("<<< [Parent Process Completed]\n\n");
    }

    return EXIT_SUCCESS;
}
