# Operating Systems Viva & Exam Prep: Processes

## 1. What is a Process?
A **process** is a program in execution. Unlike a passive program stored as binary instructions on a disk, a process is an active entity loaded into RAM with its own dedicated virtual address space, program counter, register states, stack, and file descriptors.

---

## 2. Process States & Lifecycle
A process transitions through five primary states during its execution:
```
  [ New ] ---> [ Ready ] <=======> [ Running ] ---> [ Terminated ]
                  ^                     |
                  |                     v
                  +---- [ Waiting ] <---+
```
1. **New:** The process is being created and its Process Control Block (PCB) is being allocated.
2. **Ready:** The process is loaded in main memory and waiting to be assigned to a CPU core by the scheduler.
3. **Running:** The instructions of the process are actively executing on a CPU core.
4. **Waiting (Blocked):** The process is suspended waiting for an external event (such as I/O completion, pipe data, or a timer).
5. **Terminated:** The process has finished execution and waits for its parent to read its exit status.

---

## 3. What is a Process Control Block (PCB)?
The **Process Control Block** (also called `task_struct` in the Linux kernel) is the data structure maintained by the OS kernel for each active process. It contains:
- **Process ID (PID):** Unique integer identifier.
- **Process State:** Current state (Ready, Running, Sleeping, Zombie).
- **Program Counter (PC):** Address of the next machine instruction to execute.
- **CPU Registers:** Accumulator, index registers, stack pointer saved during context switches.
- **CPU Scheduling Info:** Priority, scheduler queue pointers.
- **Memory Management Info:** Page table pointers, segment limits.
- **Accounting Info:** CPU execution time elapsed, time limits.
- **I/O Status Info:** List of allocated I/O devices and open file descriptor table.

---

## 4. How does `fork()` work?
The `fork()` system call duplicates the calling process:
- It creates a **child process** that is an exact replica of the parent process, containing copies of the parent's address space, file descriptor table, and environment.
- **Return Values:**
  - In the **parent process**: returns the PID of the newly created child.
  - In the **child process**: returns `0`.
  - On **failure**: returns `-1` (e.g. if the system exceeds maximum process limits).
- **Copy-on-Write (COW):** Modern Linux kernels do not immediately physically duplicate all memory pages on `fork()`. Instead, parent and child share the same physical pages marked as read-only. Only when either process attempts to write to a page does the kernel allocate and copy a private physical frame.

---

## 5. What is `wait()` and why is it essential?
The `wait()` and `waitpid()` system calls suspend the parent process until one of its child processes terminates.
- **Exit Status Extraction:** Macros such as `WIFEXITED(status)` and `WEXITSTATUS(status)` extract the child's return code.
- **Resource Reaping:** Calling `wait()` frees the terminated child's PCB entry from the kernel's process table.

---

## 6. What are Zombie and Orphan Processes?

### Zombie Process (`<defunct>`)
- **Definition:** A process that has completed execution via `exit()`, but whose parent has not yet called `wait()` to read its exit status.
- **Consequence:** The process consumes no CPU or memory, but holds a slot in the operating system's process table. If many zombies accumulate, the system runs out of PIDs.
- **Prevention:** The parent must call `wait()` / `waitpid()`, or handle the `SIGCHLD` signal.

### Orphan Process
- **Definition:** A child process whose parent process terminated before the child finished.
- **Resolution:** The Linux kernel automatically re-parents the orphan process to the root init system (`PID 1`, typically `systemd`). The init process periodically reaps terminated orphans.

---

## 7. How does the `exec()` family differ from `fork()`?
- `fork()` **creates a new process** by duplicating the caller.
- `exec()` (`execl`, `execv`, `execvp`, etc.) **replaces the current process image** with a completely new executable program.
- When `exec()` is called, the current code, heap, and stack segments are discarded and replaced with the target program; the PID remains unchanged.
