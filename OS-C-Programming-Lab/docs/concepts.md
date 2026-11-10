# Fundamental Operating Systems Concepts

This guide provides theoretical explanations for the Operating Systems concepts implemented throughout this laboratory repository.

---

## 1. The Operating System: Definition and Core Duties
An **Operating System (OS)** is system software that acts as an intermediary between computer hardware and user applications. Its primary objectives are:
1. **Resource Abstraction:** Providing clean, uniform APIs (system calls) so applications do not need to manage raw disk tracks, network packets, or memory timings.
2. **Resource Management:** Multiplexing CPU, RAM, and I/O devices fairly and efficiently among competing processes.
3. **Protection and Security:** Enforcing process isolation, access control, and memory boundaries via hardware privilege rings (Kernel Mode vs User Mode).

---

## 2. Processes vs Threads

### Process (Heavyweight Execution Unit)
A process is a program in execution with a completely private virtual address space.
- **Components:**
  - **Text Segment:** Compiled machine instructions.
  - **Data/BSS Segments:** Global and static variables.
  - **Heap:** Dynamically allocated memory.
  - **Stack:** Local variables, function return pointers.
  - **Process Control Block (PCB):** Kernel metadata (PID, state, CPU registers, open file descriptors).

### Thread (Lightweight Execution Unit)
A thread represents an independent thread of execution within a parent process.
- **Shared State:** Code, data, heap, open files, signal handlers.
- **Private State:** Thread ID (TID), Program Counter (PC), CPU registers, private stack.
- **Benefit:** Creating and context-switching threads requires substantially less CPU and memory overhead than spawning processes.

---

## 3. Inter-Process Communication (IPC)

Because modern operating systems isolate process address spaces, processes cannot directly read each other's memory. IPC mechanisms provide coordinated data exchange:

### A. UNIX Pipes
- Unidirectional byte streams managed inside kernel memory.
- Created via `pipe(pipefd)`.
- Follows FIFO order. Data is consumed as it is read.
- Data transfer involves two copies: **User Space $\to$ Kernel Buffer $\to$ User Space**.

### B. POSIX Shared Memory
- Maps physical memory pages directly into the virtual address spaces of two or more independent processes.
- Created via `shm_open()` and mapped via `mmap()`.
- **Zero-Copy IPC:** Once mapped, reads and writes occur at hardware memory bus speeds without traversing kernel space.
- Requires synchronization primitives (mutexes, semaphores) to prevent race conditions.

---

## 4. CPU Scheduling Theory

The CPU scheduler determines which ready process or thread receives CPU execution time when a core becomes idle.

### Scheduling Metrics
- **Arrival Time ($AT$):** Time at which a process enters the ready queue.
- **Burst Time ($BT$):** Total duration of CPU execution required by a process.
- **Completion Time ($CT$):** Exact time a process completes execution.
- **Turnaround Time ($TAT$):** Total time spent from arrival to completion ($TAT = CT - AT$).
- **Waiting Time ($WT$):** Total time spent waiting in the ready queue ($WT = TAT - BT$).

### Evaluated Algorithms:
1. **FCFS (First Come First Served):**
   - Non-preemptive. Allocates CPU in order of arrival.
   - Suffer from the **Convoy Effect**, where short jobs wait behind a massive CPU burst.
2. **SJF (Shortest Job First):**
   - Non-preemptive. Selects process with smallest CPU burst.
   - Mathematically provable to produce minimum average waiting time.
   - Vulnerable to **Starvation** of long processes if short processes continuously arrive.
3. **Round Robin (RR):**
   - Preemptive. Allocates a fixed time slice (Time Quantum $Q$) to each process cyclically.
   - Ideal for interactive systems; ensures bounded response times.

---

## 5. Synchronization & Race Conditions

When multiple execution threads read and write shared data concurrently without coordination:
- An interleaved context switch can corrupt shared state (**Race Condition**).
- The section of code modifying the shared resource is called the **Critical Section**.

### Synchronization Primitives:
- **Mutex (Mutual Exclusion Lock):**
  - Binary lock with strict ownership. Only the thread holding the lock can release it.
- **Semaphore:**
  - Generalized signaling mechanism invented by Edsger Dijkstra.
  - Maintains an integer count of available resource permits.
  - Can be Binary ($0, 1$) or Counting ($N$ units).
- **Producer-Consumer Problem:**
  - Classic concurrency challenge where producers and consumers coordinate over a bounded buffer using counting semaphores and mutex locks without buffer overflow, buffer underflow, or deadlock.
