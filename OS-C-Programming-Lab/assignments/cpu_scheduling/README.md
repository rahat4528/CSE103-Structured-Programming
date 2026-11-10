# CPU Scheduling Algorithms

This folder implements and simulates three fundamental CPU scheduling algorithms: **First Come First Served (FCFS)**, **Shortest Job First (SJF - Non-Preemptive)**, and **Round Robin (RR)**.

> [!NOTE]
> **Educational Simulation Disclaimer:**
> These programs are educational simulations designed to demonstrate theoretical CPU scheduling principles taught in Operating Systems courses. They do **not** implement the actual Linux kernel scheduler (the Linux kernel employs the Completely Fair Scheduler / EEVDF scheduler with red-black runqueues, priority nice levels, and virtual runtime).
>
> **Simplifying Assumption:**
> For educational clarity and straightforward mathematical verification, all processes are assumed to arrive at time zero ($AT = 0$).

---

## Benchmark Test Dataset & Mathematical Verification

All three programs use this standardized benchmark dataset:
- $P_1$: Burst Time = $6\text{ ms}$
- $P_2$: Burst Time = $8\text{ ms}$
- $P_3$: Burst Time = $7\text{ ms}$
- $P_4$: Burst Time = $3\text{ ms}$

### Comparative Summary:

| Algorithm | Execution Sequence | Average Waiting Time | Average Turnaround Time |
|---|---|---|---|
| **FCFS** | $P_1 \to P_2 \to P_3 \to P_4$ | **$10.25\text{ ms}$** | **$16.25\text{ ms}$** |
| **SJF (Non-preemptive)** | $P_4 \to P_1 \to P_3 \to P_2$ | **$7.00\text{ ms}$** *(Optimal)* | **$13.00\text{ ms}$** *(Optimal)* |
| **Round Robin ($Q=4$)** | $P_1 \to P_2 \to P_3 \to P_4 \to P_1 \to P_2 \to P_3$ | **$13.25\text{ ms}$** | **$19.25\text{ ms}$** |

---

## 1. fcfs.c — First Come First Served

- **Program Name:** `fcfs.c`
- **OS Concept:** Non-Preemptive FIFO Scheduling, Convoy Effect.
- **Objective:** Allocate the CPU in arrival order, calculate completion time, turnaround time, and waiting time, and visualize execution using an ASCII Gantt chart.
- **Required Headers:** `<stdio.h>`, `<stdlib.h>`, `<string.h>`
- **Important Formulas:**
  - $\text{Completion Time } (CT_i) = CT_{i-1} + BT_i$
  - $\text{Turnaround Time } (TAT) = CT - AT = CT$ (since $AT = 0$)
  - $\text{Waiting Time } (WT) = TAT - BT$
- **How It Works:**
  1. Executes processes in index order $P_1 \to P_2 \to P_3 \to P_4$.
  2. Accumulates elapsed burst times into `completion_time`.
  3. Computes $TAT$ and $WT$ per process.
  4. Prints Gantt chart and statistical averages.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic assignments/cpu_scheduling/fcfs.c -o build/assignments/cpu_scheduling/fcfs
  ```
- **Execution Command:**
  ```bash
  ./build/assignments/cpu_scheduling/fcfs           # Default verified dataset
  ./build/assignments/cpu_scheduling/fcfs --custom  # Interactive keyboard input
  ```
- **Expected Output:**
  ```text
  ====================================================
       CPU Scheduling Simulation: FCFS (Non-preemptive) 
  ====================================================
  [Note] Educational model. All processes arrive at time t = 0.

  [Mode] Running with standard educational test dataset:

  --- GANTT CHART ---
  |   P1   |   P2   |   P3   |   P4   |
  0        6       14       21       24

  ----------------- SCHEDULING RESULTS -----------------
  | PID | Arrival | Burst Time | Completion | TAT | WT |
  |-----|---------|------------|------------|-----|----|
  | P1  |    0    |      6     |      6     |   6 |  0 |
  | P2  |    0    |      8     |     14     |  14 |  6 |
  | P3  |    0    |      7     |     21     |  21 | 14 |
  | P4  |    0    |      3     |     24     |  24 | 21 |
  ------------------------------------------------------
  Average Waiting Time    : 10.25 ms
  Average Turnaround Time : 16.25 ms
  ====================================================
  ```
- **Common Errors:**
  - Confusing Waiting Time with Turnaround Time ($WT = TAT - BT$, whereas $TAT = CT - AT$).

---

## 2. sjf.c — Shortest Job First (Non-preemptive)

- **Program Name:** `sjf.c`
- **OS Concept:** Greedy Scheduling, Minimization of Average Waiting Time, Starvation Risk.
- **Objective:** Sort processes by CPU burst time, schedule shortest jobs first, and prove mathematically that SJF minimizes average waiting time.
- **Required Headers:** `<stdio.h>`, `<stdlib.h>`, `<string.h>`
- **Important Functions:**
  - `qsort()`: Standard C library quicksort sorting processes in ascending burst time order.
- **How It Works:**
  1. Inspects burst times and sorts processes: $P_4(3) \to P_1(6) \to P_3(7) \to P_2(8)$.
  2. Computes sequential completion times.
  3. Displays ASCII Gantt chart reflecting the reordered execution schedule.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic assignments/cpu_scheduling/sjf.c -o build/assignments/cpu_scheduling/sjf
  ```
- **Execution Command:**
  ```bash
  ./build/assignments/cpu_scheduling/sjf
  ```
- **Expected Output:**
  ```text
  ====================================================
       CPU Scheduling Simulation: SJF (Non-preemptive) 
  ====================================================
  [Note] Educational model. All processes arrive at time t = 0.

  [Mode] Running with standard educational test dataset:

  --- GANTT CHART (Execution Order) ---
  |   P4   |   P1   |   P3   |   P2   |
  0        3        9       16       24

  ----------------- SCHEDULING RESULTS -----------------
  | PID | Arrival | Burst Time | Completion | TAT | WT |
  |-----|---------|------------|------------|-----|----|
  | P4  |    0    |      3     |      3     |   3 |  0 |
  | P1  |    0    |      6     |      9     |   9 |  3 |
  | P3  |    0    |      7     |     16     |  16 |  9 |
  | P2  |    0    |      8     |     24     |  24 | 16 |
  ------------------------------------------------------
  Average Waiting Time    : 7.00 ms
  Average Turnaround Time : 13.00 ms
  ====================================================
  ```
- **Common Errors:**
  - Starvation: In dynamic systems, continuous arrival of short jobs starves long jobs indefinitely.

---

## 3. round_robin.c — Round Robin

- **Program Name:** `round_robin.c`
- **OS Concept:** Preemptive Time-Sliced Scheduling, Ready Queue Simulation, Context Switching Trade-offs.
- **Objective:** Implement a round-robin circular queue with time quantum $Q=4\text{ ms}$, track remaining execution times across slices, and log time slice intervals in the Gantt chart.
- **Required Headers:** `<stdio.h>`, `<stdlib.h>`, `<string.h>`
- **Important Functions & Structures:**
  - `Process.remaining_time`: Decremented dynamically until zero.
  - `GanttSlice`: Captures discrete scheduling slices.
- **How It Works:**
  1. Initializes $Q = 4\text{ ms}$ and copies burst times to `remaining_time`.
  2. Iterates across ready processes. If `remaining_time > Q`, consumes $Q$ time and leaves process in queue.
  3. If `remaining_time <= Q`, consumes remainder, records `completion_time`, and increments `completed`.
  4. Continues until all processes finish.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic assignments/cpu_scheduling/round_robin.c -o build/assignments/cpu_scheduling/round_robin
  ```
- **Execution Command:**
  ```bash
  ./build/assignments/cpu_scheduling/round_robin
  ```
- **Expected Output:**
  ```text
  ====================================================
       CPU Scheduling Simulation: Round Robin (RR)    
  ====================================================
  [Note] Educational model. All processes arrive at time t = 0.

  [Mode] Running with standard educational test dataset:
    Time Quantum: 4 ms

  --- GANTT CHART (Time Slices) ---
  |  P1  |  P2  |  P3  |  P4  |  P1  |  P2  |  P3  |
  0      4      8     12     15     17     21     24

  ----------------- SCHEDULING RESULTS -----------------
  | PID | Arrival | Burst Time | Completion | TAT | WT |
  |-----|---------|------------|------------|-----|----|
  | P1  |    0    |      6     |     17     |  17 | 11 |
  | P2  |    0    |      8     |     21     |  21 | 13 |
  | P3  |    0    |      7     |     24     |  24 | 17 |
  | P4  |    0    |      3     |     15     |  15 | 12 |
  ------------------------------------------------------
  Time Quantum            : 4 ms
  Average Waiting Time    : 13.25 ms
  Average Turnaround Time : 19.25 ms
  ====================================================
  ```
- **Common Errors:**
  - Setting quantum too small: causes excessive context-switch overhead in real OS kernels.
  - Setting quantum too large: Round Robin degenerates into FCFS.
