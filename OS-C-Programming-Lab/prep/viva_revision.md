# Operating Systems Comprehensive Viva & Oral Exam Cheat Sheet

This rapid revision guide covers high-frequency questions asked in Operating Systems university laboratory vivas, practical exams, and technical interviews.

---

## 1. High-Frequency Viva Questions & Concise Answers

### Q1: What happens under the hood when `fork()` is called?
> **Answer:** The Linux kernel creates a new task struct (PCB), duplicates the parent's memory page tables in Copy-on-Write (COW) mode, copies file descriptor references, assigns a unique PID, and inserts the child into the scheduler's runqueue. `fork()` returns the child's PID to the parent, and `0` to the child.

### Q2: What is the difference between a system call and a standard library function?
> **Answer:** A **system call** (`read`, `write`, `fork`) is an interface to request kernel-space privileged services, triggering a CPU software interrupt / CPU mode switch from user mode (Ring 3) to kernel mode (Ring 0). A **library function** (`printf`, `strcpy`) executes primarily in user space; some library functions wrap system calls with user-level buffering (e.g. `fwrite` wraps `write`).

### Q3: Why is closing unused pipe descriptors mandatory?
> **Answer:** If the reader process leaves its write end of the pipe open, `read()` will block indefinitely waiting for EOF even after the sender has finished and exited. This causes a permanent **pipe deadlock**.

### Q4: Why must we link `-pthread` rather than just `#include <pthread.h>`?
> **Answer:** `#include <pthread.h>` only provides function prototypes and types to the compiler during parsing. The `-pthread` flag instructs the compiler and linker to link against the POSIX threads runtime library (`libpthread`) and defines internal thread-safety reentrancy macros (`_REENTRANT`).

### Q5: How does POSIX shared memory differ from UNIX pipes?
> **Answer:** Pipes are kernel-buffered byte streams requiring data to be copied twice (User $\to$ Kernel $\to$ User). POSIX shared memory maps physical memory frames directly into the virtual address spaces of both processes via `mmap()`, enabling **zero-copy** communication at raw memory speeds.

### Q6: Can a semaphore have a negative value?
> **Answer:** In theoretical Dijkstra counting semaphores, a negative value denotes the number of processes waiting in the queue. In POSIX semaphores (`sem_t`), `sem_getvalue()` never returns a negative number; it returns 0 when the semaphore is exhausted.

---

## 2. Frequently Confused Concepts

| Concept A | Concept B | Key Distinction |
|---|---|---|
| **Process** | **Thread** | A process has an isolated virtual address space; threads within a process share the same address space. |
| **Zombie Process** | **Orphan Process** | A zombie has terminated but its parent hasn't called `wait()`. An orphan's parent terminated while the child is still actively running. |
| **Mutex** | **Binary Semaphore** | A mutex has strict ownership (only locking thread can unlock); a semaphore can be posted/unlocked by any thread or signal. |
| **Stack** | **Heap** | Stack memory is automatically managed in LIFO order by CPU instructions; Heap memory is explicitly requested and managed by programmer. |
| **FCFS** | **Round Robin** | FCFS is non-preemptive and suffers from the convoy effect; Round Robin is preemptive based on a cyclic time quantum. |
| **Starvation** | **Deadlock** | In starvation, processes are ready to run but indefinitely delayed by higher priorities. In deadlock, processes are permanently blocked waiting on each other. |

---

## 3. Common C Compilation & Runtime Errors in OS Labs

### 1. `undefined reference to 'pthread_create'`
- **Cause:** Missing `-pthread` flag at compilation/linking.
- **Fix:** Compile with `gcc -pthread program.c -o program`.

### 2. `undefined reference to 'shm_open'`
- **Cause:** POSIX Realtime Extensions library not linked.
- **Fix:** Append `-lrt` to the compiler command: `gcc program.c -o program -lrt`.

### 3. `Segmentation Fault (Core Dumped)`
- **Common OS Causes:**
  - Dereferencing a NULL pointer returned by failed `malloc()` or `fopen()`.
  - Writing to memory mapped with `PROT_READ`.
  - Buffer overflow on stack or heap.

### 4. `Bus Error (SIGBUS)`
- **Common OS Cause:**
  - Memory mapping a shared memory object without first calling `ftruncate()` to size the file.

### 5. `Broken Pipe (SIGPIPE)`
- **Cause:** Writing to a pipe or socket whose read end has already been closed by all reading processes.

