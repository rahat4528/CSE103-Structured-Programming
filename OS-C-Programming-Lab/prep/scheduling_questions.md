# Operating Systems Viva & Exam Prep: CPU Scheduling

## 1. Scheduling Criteria & Performance Metrics

Operating system scheduling algorithms are evaluated using five primary quantitative criteria:

1. **CPU Utilization:** Percentage of time the CPU is actively performing useful user/system work (ideally 40% to 90%).
2. **Throughput:** Number of processes completed per unit time.
3. **Turnaround Time (TAT):** Total time interval from process submission to process termination.
   $$\text{TAT} = \text{Completion Time (CT)} - \text{Arrival Time (AT)}$$
4. **Waiting Time (WT):** Total time a process spends waiting in the ready queue.
   $$\text{WT} = \text{Turnaround Time (TAT)} - \text{Burst Time (BT)}$$
5. **Response Time:** Time from submission until the first response is produced (critical for interactive GUI and terminal systems).

---

## 2. Preemptive vs Non-Preemptive Scheduling

| Dimension | Non-Preemptive Scheduling | Preemptive Scheduling |
|---|---|---|
| **Core Concept** | Once a process gets the CPU, it retains control until it terminates or blocks for I/O | The OS can interrupt a running process and reallocate CPU to a higher-priority task |
| **Context Switch Overhead** | Low (switches occur only at process boundaries) | High (frequent timer interrupts and context save/restore) |
| **Response Time** | High / Sluggish for short jobs arriving behind long ones | Low / Responsive for interactive and real-time tasks |
| **Shared Data Risks** | Lower risk of race conditions | High risk; requires mutexes to protect shared kernel data |
| **Examples** | Standard FCFS, Non-preemptive SJF | Round Robin, SRTF, Linux CFS |

---

## 3. Algorithm Catalog & Characteristics

### 1. First-Come, First-Served (FCFS)
- **Nature:** Non-preemptive.
- **Advantage:** Trivial to implement using a FIFO queue. Zero starvation.
- **Disadvantage:** Severe **Convoy Effect** — a long CPU-bound process blocks multiple short I/O-bound processes, collapsing CPU and device utilization.

### 2. Shortest Job First (SJF / SRTF)
- **Nature:** Non-preemptive (SJF) or Preemptive (Shortest Remaining Time First - SRTF).
- **Theorem:** **Provably optimal** in terms of minimizing average waiting time for a given set of stationary processes.
- **Disadvantages:**
  - Cannot know CPU burst length in advance (must predict via exponential smoothing $\tau_{n+1} = \alpha t_n + (1-\alpha)\tau_n$).
  - **Starvation:** Long processes may wait indefinitely if short processes arrive continuously.

### 3. Round Robin (RR)
- **Nature:** Preemptive with fixed Time Quantum ($Q$).
- **Mechanism:** Ready queue is treated as a circular FIFO buffer. Each process runs for at most $Q$ units before timer interrupt triggers preemption.
- **Quantum Trade-Offs:**
  - $Q \to \infty$: Round Robin degenerates into FCFS.
  - $Q \to 0$: Creates the illusion of simultaneous execution ("processor sharing"), but excessive context-switching overhead destroys actual throughput.
  - *Rule of thumb:* 80% of CPU bursts should be shorter than the time quantum $Q$.

### 4. Multilevel Feedback Queue (MLFQ)
- Modern general-purpose scheduling architecture:
  - Multiple priority queues with different time quanta (e.g. $Q=10\text{ ms}$, $Q=20\text{ ms}$, FCFS).
  - Processes that use their full quantum are demoted to lower-priority queues (CPU-bound penalty).
  - Processes that yield voluntarily for I/O stay in higher-priority queues (interactive boost).
  - Includes **Aging** (periodically boosting priority) to guarantee starvation prevention.
