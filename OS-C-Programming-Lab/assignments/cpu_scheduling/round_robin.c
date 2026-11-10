/**
 * @file round_robin.c
 * @brief Round Robin (RR) Preemptive CPU Scheduling Simulation.
 *
 * Educational Simulation Note:
 *  - Educational simulation demonstrating time-sliced preemptive queue scheduling.
 *  - Assumption: All processes arrive at time zero (AT = 0).
 *  - Ready queue cycles through ready processes giving each up to time quantum Q.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 10
#define MAX_SLICES 100

typedef struct {
    int pid;
    int burst_time;
    int remaining_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
} Process;

typedef struct {
    int pid;
    int end_time;
} GanttSlice;

int main(int argc, char *argv[]) {
    printf("====================================================\n");
    printf("     CPU Scheduling Simulation: Round Robin (RR)    \n");
    printf("====================================================\n");
    printf("[Note] Educational model. All processes arrive at time t = 0.\n\n");

    int n = 4;
    int quantum = 4;
    Process proc[MAX_PROCESSES];

    int use_default = 1;
    if (argc > 1 && strcmp(argv[1], "--custom") == 0) {
        use_default = 0;
    }

    if (use_default) {
        printf("[Mode] Running with standard educational test dataset:\n");
        printf("  Time Quantum: %d ms\n", quantum);
        proc[0] = (Process){.pid = 1, .burst_time = 6, .remaining_time = 6};
        proc[1] = (Process){.pid = 2, .burst_time = 8, .remaining_time = 8};
        proc[2] = (Process){.pid = 3, .burst_time = 7, .remaining_time = 7};
        proc[3] = (Process){.pid = 4, .burst_time = 3, .remaining_time = 3};
    } else {
        printf("Enter number of processes (1 - %d): ", MAX_PROCESSES);
        if (scanf("%d", &n) != 1 || n <= 0 || n > MAX_PROCESSES) {
            fprintf(stderr, "Invalid process count.\n");
            return EXIT_FAILURE;
        }
        for (int i = 0; i < n; i++) {
            proc[i].pid = i + 1;
            printf("Enter Burst Time for Process P%d: ", proc[i].pid);
            if (scanf("%d", &proc[i].burst_time) != 1 || proc[i].burst_time <= 0) {
                fprintf(stderr, "Invalid burst time.\n");
                return EXIT_FAILURE;
            }
            proc[i].remaining_time = proc[i].burst_time;
        }
        printf("Enter Time Quantum: ");
        if (scanf("%d", &quantum) != 1 || quantum <= 0) {
            fprintf(stderr, "Invalid quantum.\n");
            return EXIT_FAILURE;
        }
    }

    // Ready Queue Simulation
    GanttSlice slices[MAX_SLICES];
    int slice_count = 0;
    int current_time = 0;
    int completed = 0;

    while (completed < n) {
        int progress_made = 0;

        for (int i = 0; i < n; i++) {
            if (proc[i].remaining_time > 0) {
                progress_made = 1;

                if (proc[i].remaining_time > quantum) {
                    current_time += quantum;
                    proc[i].remaining_time -= quantum;
                } else {
                    current_time += proc[i].remaining_time;
                    proc[i].remaining_time = 0;
                    proc[i].completion_time = current_time;
                    completed++;
                }

                if (slice_count < MAX_SLICES) {
                    slices[slice_count].pid = proc[i].pid;
                    slices[slice_count].end_time = current_time;
                    slice_count++;
                }
            }
        }

        if (!progress_made) {
            break;
        }
    }

    // Compute metrics
    double total_wt = 0.0;
    double total_tat = 0.0;

    for (int i = 0; i < n; i++) {
        proc[i].turnaround_time = proc[i].completion_time; // AT = 0
        proc[i].waiting_time = proc[i].turnaround_time - proc[i].burst_time;
        total_wt += proc[i].waiting_time;
        total_tat += proc[i].turnaround_time;
    }

    // Display Gantt Chart
    printf("\n--- GANTT CHART (Time Slices) ---\n|");
    for (int i = 0; i < slice_count; i++) {
        printf("  P%d  |", slices[i].pid);
    }
    printf("\n0");
    for (int i = 0; i < slice_count; i++) {
        printf("     %2d", slices[i].end_time);
    }
    printf("\n\n");

    // Display Results Table
    printf("----------------- SCHEDULING RESULTS -----------------\n");
    printf("| PID | Arrival | Burst Time | Completion | TAT | WT |\n");
    printf("|-----|---------|------------|------------|-----|----|\n");
    for (int i = 0; i < n; i++) {
        printf("| P%-2d |    0    |     %2d     |     %2d     | %3d | %2d |\n",
               proc[i].pid, proc[i].burst_time, proc[i].completion_time,
               proc[i].turnaround_time, proc[i].waiting_time);
    }
    printf("------------------------------------------------------\n");

    printf("Time Quantum            : %d ms\n", quantum);
    printf("Average Waiting Time    : %.2f ms\n", total_wt / n);
    printf("Average Turnaround Time : %.2f ms\n", total_tat / n);
    printf("====================================================\n");

    return EXIT_SUCCESS;
}

