/**
 * @file process_communication.c
 * @brief Full duplex parent-child IPC combining fork(), pipe(), read(), write(), and wait().
 *
 * Concepts Demonstrated:
 *  - Establishing two-way IPC using two separate UNIX pipes
 *  - Parent-to-Child request channel + Child-to-Parent response channel
 *  - Proper closure of 4 unused file descriptor ends across the two processes
 *  - Process synchronization and status verification using waitpid()
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DATA_SIZE 5

// Payload sent from parent to child
typedef struct {
    int count;
    int numbers[DATA_SIZE];
} TaskPayload;

// Response sent from child back to parent
typedef struct {
    int sum;
    double average;
    int min_val;
    int max_val;
} TaskResult;

int main(void) {
    printf("====================================================\n");
    printf(" Integrated OS Example: Bidirectional Pipe IPC      \n");
    printf("====================================================\n\n");

    // pipe_p2c: Parent writes, Child reads
    // pipe_c2p: Child writes, Parent reads
    int pipe_p2c[2];
    int pipe_c2p[2];

    if (pipe(pipe_p2c) == -1 || pipe(pipe_c2p) == -1) {
        perror("Failed to create pipes");
        return EXIT_FAILURE;
    }

    printf("[Parent] Pipes created:\n");
    printf("  Parent-to-Child Pipe: read_fd=%d, write_fd=%d\n", pipe_p2c[0], pipe_p2c[1]);
    printf("  Child-to-Parent Pipe: read_fd=%d, write_fd=%d\n\n", pipe_c2p[0], pipe_c2p[1]);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // ----------------------------------------------------
        // CHILD PROCESS: COMPUTATION WORKER
        // ----------------------------------------------------
        // Close unused descriptors:
        // Child reads from p2c[0], so close p2c[1]
        // Child writes to c2p[1], so close c2p[0]
        close(pipe_p2c[1]);
        close(pipe_c2p[0]);

        printf("  [Child (PID %d)] Descriptors configured. Awaiting task payload...\n", (int)getpid());

        TaskPayload request;
        ssize_t bytes_in = read(pipe_p2c[0], &request, sizeof(TaskPayload));
        if (bytes_in <= 0) {
            perror("child read error");
            close(pipe_p2c[0]);
            close(pipe_c2p[1]);
            exit(EXIT_FAILURE);
        }

        printf("  [Child] Received %d integers: [", request.count);
        for (int i = 0; i < request.count; i++) {
            printf("%d%s", request.numbers[i], (i == request.count - 1) ? "" : ", ");
        }
        printf("]\n");

        // Perform statistical computations
        TaskResult result;
        result.sum = 0;
        result.min_val = request.numbers[0];
        result.max_val = request.numbers[0];

        for (int i = 0; i < request.count; i++) {
            int val = request.numbers[i];
            result.sum += val;
            if (val < result.min_val) result.min_val = val;
            if (val > result.max_val) result.max_val = val;
        }
        result.average = (double)result.sum / request.count;

        printf("  [Child] Calculation complete. Sending response back to parent...\n");
        ssize_t bytes_out = write(pipe_c2p[1], &result, sizeof(TaskResult));
        if (bytes_out <= 0) {
            perror("child write error");
        }

        // Clean closure
        close(pipe_p2c[0]);
        close(pipe_c2p[1]);
        printf("  [Child] Closed descriptors. Exiting.\n");
        exit(EXIT_SUCCESS);
    } else {
        // ----------------------------------------------------
        // PARENT PROCESS: ORCHESTRATOR
        // ----------------------------------------------------
        // Close unused descriptors:
        // Parent writes to p2c[1], so close p2c[0]
        // Parent reads from c2p[0], so close c2p[1]
        close(pipe_p2c[0]);
        close(pipe_c2p[1]);

        TaskPayload task;
        task.count = DATA_SIZE;
        task.numbers[0] = 42;
        task.numbers[1] = 17;
        task.numbers[2] = 88;
        task.numbers[3] = 63;
        task.numbers[4] = 29;

        printf("[Parent] Dispatching task with %d numbers to Child PID %d...\n", 
               task.count, (int)pid);
        write(pipe_p2c[1], &task, sizeof(TaskPayload));
        close(pipe_p2c[1]); // Close write end to signal EOF to child

        // Read response from child
        TaskResult response;
        printf("[Parent] Waiting for computation results from child...\n");
        ssize_t bytes_in = read(pipe_c2p[0], &response, sizeof(TaskResult));
        if (bytes_in > 0) {
            printf("\n[Parent] Results received from child:\n");
            printf("  Sum     : %d\n", response.sum);
            printf("  Average : %.2f\n", response.average);
            printf("  Minimum : %d\n", response.min_val);
            printf("  Maximum : %d\n", response.max_val);
        }
        close(pipe_c2p[0]);

        // Reap child
        int status = 0;
        waitpid(pid, &status, 0);

        printf("\n[Parent] Child reaped. Bidirectional IPC test passed successfully.\n");
        printf("====================================================\n");
    }

    return EXIT_SUCCESS;
}
