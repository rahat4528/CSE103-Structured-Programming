# Process Creation Practice

This folder covers foundational process creation, lifecycle management, and synchronization in Linux using the POSIX system call interface.

---

## 1. fork_example.c

- **Program Name:** `fork_example.c`
- **OS Concept:** Process Creation, Process Identifiers (PID, PPID), and Address Space Duplication (Copy-on-Write).
- **Objective:** Demonstrate how a parent process spawns a child process using `fork()`, observe process hierarchy via `getpid()` and `getppid()`, and prove memory space isolation.
- **Required Headers:**
  - `<stdio.h>`: Standard I/O functions.
  - `<stdlib.h>`: Memory management and process termination macros (`EXIT_SUCCESS`, `exit`).
  - `<unistd.h>`: POSIX API (`fork`, `getpid`, `getppid`, `sleep`).
  - `<sys/types.h>`: Type definitions such as `pid_t`.
  - `<sys/wait.h>`: `waitpid()` to cleanly reap the child after parent demonstrates independence.
- **Important Functions:**
  - `fork()`: Creates a new child process by duplicating the calling process.
  - `getpid()`: Returns the process ID of the calling process.
  - `getppid()`: Returns the process ID of the parent process.
  - `waitpid()`: Reaps the child process at the end of parent execution to avoid zombie processes.
- **How It Works:**
  1. The parent process initializes a local integer variable `shared_variable = 100`.
  2. `fork()` is invoked. The kernel creates a child process with a separate address space.
  3. `fork()` returns `0` inside the child process, and returns the child's PID inside the parent process.
  4. The child modifies its copy of `shared_variable` to `150`.
  5. The parent reads `shared_variable` and observes it is still `100`, proving memory isolation.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic practice/process_creation/fork_example.c -o build/practice/process_creation/fork_example
  ```
- **Execution Command:**
  ```bash
  ./build/practice/process_creation/fork_example
  ```
- **Expected Output:**
  ```text
  ====================================================
         Process Creation Demonstration: fork()       
  ====================================================

  [Initial State] Before fork():
    Parent Process PID : 10123
    shared_variable    : 100 (Address: 0x7ffd5342a12c)

  Calling fork()...

  >>> [Child Process Branch]
    Returned PID from fork() : 0
    Child PID (getpid)       : 10124
    Parent PID (getppid)     : 10123
    Child sees shared_variable = 100
    Child modified shared_variable to: 150
    Child variable address   : 0x7ffd5342a12c
  >>> [Child Process Completed]

  <<< [Parent Process Branch]
    Returned PID from fork() : 10124 (Child's PID)
    Parent PID (getpid)      : 10123
    Grandparent PID (getppid): 9980
    Parent sees shared_variable = 100
    Notice: shared_variable remains 100 in the parent!
    This proves the child received an isolated copy of memory.
  <<< [Parent Process Completed]
  ```
- **Common Errors:**
  - Expecting changes made in the child process to update the parent's memory (they are distinct address spaces).
  - Failing to check if `fork() < 0` when the system reaches process limits (`EAGAIN` / `ENOMEM`).

---

## 2. fork_wait.c

- **Program Name:** `fork_wait.c`
- **OS Concept:** Process Synchronization, Process Reaping, and Zombie Prevention.
- **Objective:** Synchronize parent and child process lifecycles using `wait()`, inspect child termination status, and prevent defunct/zombie processes.
- **Required Headers:**
  - `<stdio.h>`
  - `<stdlib.h>`
  - `<unistd.h>`
  - `<sys/types.h>`
  - `<sys/wait.h>`: Definitions for `wait`, `waitpid`, and status macros (`WIFEXITED`, `WEXITSTATUS`).
- **Important Functions:**
  - `fork()`: Spawns the child process.
  - `wait(int *status)`: Suspends execution of the parent process until one of its children terminates.
  - `WIFEXITED(status)`: Evaluates to true if the child terminated normally.
  - `WEXITSTATUS(status)`: Extracts the 8-bit exit code passed by the child to `exit()`.
- **How It Works:**
  1. The parent spawns a child process.
  2. The child performs a 3-step simulated task and calls `exit(42)`.
  3. The parent suspends on `wait(&status)`.
  4. Once the child finishes, the OS signals the parent and delivers the exit status.
  5. The parent reaps the child's entry from the process table, preventing a zombie state.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic practice/process_creation/fork_wait.c -o build/practice/process_creation/fork_wait
  ```
- **Execution Command:**
  ```bash
  ./build/practice/process_creation/fork_wait
  ```
- **Expected Output:**
  ```text
  ====================================================
        Process Synchronization: fork() & wait()      
  ====================================================

  [Parent] PID 10250 is creating a child process...
  [Parent] Waiting for child PID 10251 to terminate...
    [Child] PID 10251 started. Performing simulated task...
    [Child] Step 1 of 3 completed...
    [Child] Step 2 of 3 completed...
    [Child] Step 3 of 3 completed...
    [Child] Terminating voluntarily with exit code 42.

  [Parent] Child process 10251 has terminated.
  [Parent] Termination: Normal exit.
  [Parent] Reaped Exit Code : 42
  [Parent] Verification successful: Exit code matches child's sent status.
  [Parent] Zombie prevention complete. Parent exiting cleanly.
  ```
- **Common Errors:**
  - Forgetting to call `wait()` in the parent: if parent runs continuously without reaping terminated children, children remain zombies (`<defunct>`).
  - Misinterpreting the raw integer returned by `wait()` instead of using `WEXITSTATUS(status)`.

