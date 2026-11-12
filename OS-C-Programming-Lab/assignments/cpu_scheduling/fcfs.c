/**
 * @file fcfs.c
 * @brief First-Come, First-Served (FCFS) CPU Scheduling Simulation.
 *
 * Educational Simulation Note:
 *  - This is an educational algorithm simulation, not the Linux kernel CFS scheduler.
 *  - Assumption: All processes arrive at time zero (Arrival Time AT = 0).
 *
 * Calculations:
 *  - Completion Time (CT): Time when execution completes.
 *  - Turnaround Time (TAT): TAT = CT - AT = CT
 *  - Waiting Time (WT): WT = TAT - Burst Time (BT)
 */

#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 10
#define MAX_BURST_TIME 10000

/* Safe line-based integer parser using fgets() + strtol() (rejects trailing garbage). */
static int read_int_in_range(const char *prompt, int min_val, int max_val, int *out_val) {
    char line[64];
    printf("%s", prompt);
    if (fgets(line, sizeof(line), stdin) == NULL) {
        return 0;
    }

    size_t len = strlen(line);
    if (len > 0 && line[len - 1] != '\n' && !feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        return 0;
    }

    char *start = line;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    if (*start == '\0') {
        return 0;
    }

    char *endptr = NULL;
    errno = 0;
    long val = strtol(start, &endptr, 10);
    if (errno != 0 || endptr == start) {
        return 0;
    }
    while (*endptr && isspace((unsigned char)*endptr)) {
        endptr++;
    }
    if (*endptr != '\0' || val < min_val || val > max_val) {
        return 0;
    }

    *out_val = (int)val;
    return 1;
}

int main(int argc, char *argv[]) {
    printf("====================================================\n");
    printf("     CPU Scheduling Simulation: FCFS (Non-preemptive) \n");
    printf("====================================================\n");
    printf("[Note] Educational model. All processes arrive at time t = 0.\n\n");

    int use_default = 1;
    if (argc > 1) {
        if (strcmp(argv[1], "--custom") == 0) {
            use_default = 0;
        } else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printf("Usage: %s [--custom | --help]\n", argv[0]);
            printf("  (no args) : Run verified benchmark dataset (P1=6, P2=8, P3=7, P4=3)\n");
            printf("  --custom  : Prompt for custom process count and burst times\n");
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "[Error] Unknown option: %s\n", argv[1]);
            fprintf(stderr, "Usage: %s [--custom | --help]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    typedef struct {
        int pid;
        int burst_time;
        int completion_time;
        int turnaround_time;
        int waiting_time;
    } Process;

    int n = 4;
    Process proc[MAX_PROCESSES];

    if (use_default) {
        printf("[Mode] Running with standard educational test dataset:\n");
        proc[0] = (Process){.pid = 1, .burst_time = 6};
        proc[1] = (Process){.pid = 2, .burst_time = 8};
        proc[2] = (Process){.pid = 3, .burst_time = 7};
        proc[3] = (Process){.pid = 4, .burst_time = 3};
    } else {
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "Enter number of processes (1 - %d): ", MAX_PROCESSES);
        if (!read_int_in_range(prompt, 1, MAX_PROCESSES, &n)) {
            fprintf(stderr, "[Error] Invalid process count (must be 1 - %d).\n", MAX_PROCESSES);
            return EXIT_FAILURE;
        }
        for (int i = 0; i < n; i++) {
            proc[i].pid = i + 1;
            snprintf(prompt, sizeof(prompt), "Enter Burst Time for Process P%d (1 - %d): ",
                     proc[i].pid, MAX_BURST_TIME);
            if (!read_int_in_range(prompt, 1, MAX_BURST_TIME, &proc[i].burst_time)) {
                fprintf(stderr, "[Error] Invalid burst time (must be 1 - %d).\n", MAX_BURST_TIME);
                return EXIT_FAILURE;
            }
        }
    }

    // FCFS Computation
    int current_time = 0;
    double total_wt = 0.0;
    double total_tat = 0.0;

    for (int i = 0; i < n; i++) {
        current_time += proc[i].burst_time;
        proc[i].completion_time = current_time;
        proc[i].turnaround_time = proc[i].completion_time; // AT = 0
        proc[i].waiting_time = proc[i].turnaround_time - proc[i].burst_time;

        total_wt += proc[i].waiting_time;
        total_tat += proc[i].turnaround_time;
    }

    // Display ASCII Gantt Chart
    printf("\n--- GANTT CHART ---\n|");
    for (int i = 0; i < n; i++) {
        printf("   P%d   |", proc[i].pid);
    }
    printf("\n0");
    for (int i = 0; i < n; i++) {
        printf("       %2d", proc[i].completion_time);
    }
    printf("\n\n");

    // Display Scheduling Table
    printf("----------------- SCHEDULING RESULTS -----------------\n");
    printf("| PID | Arrival | Burst Time | Completion | TAT | WT |\n");
    printf("|-----|---------|------------|------------|-----|----|\n");
    for (int i = 0; i < n; i++) {
        printf("| P%-2d |    0    |     %2d     |     %2d     | %3d | %2d |\n",
               proc[i].pid, proc[i].burst_time, proc[i].completion_time,
               proc[i].turnaround_time, proc[i].waiting_time);
    }
    printf("------------------------------------------------------\n");

    printf("Average Waiting Time    : %.2f ms\n", total_wt / n);
    printf("Average Turnaround Time : %.2f ms\n", total_tat / n);
    printf("====================================================\n");

    return EXIT_SUCCESS;
}
