# Pipes Practice and Class Work

This folder covers unidirectional Inter-Process Communication (IPC) in Linux using POSIX pipes, managing kernel file descriptor pairs, and preventing pipeline deadlocks.

---

## 1. basic_pipe.c

- **Program Name:** `basic_pipe.c`
- **OS Concept:** Unidirectional Pipe IPC, File Descriptors, Kernel Buffer Streams.
- **Objective:** Create a kernel pipe using `pipe()`, explore `pipefd[0]` and `pipefd[1]`, write a message through the write descriptor, and read it from the read descriptor within a single process.
- **Required Headers:**
  - `<stdio.h>`: Standard I/O.
  - `<stdlib.h>`: Process exit macros.
  - `<string.h>`: String manipulation (`strlen`, `memset`).
  - `<unistd.h>`: POSIX system calls (`pipe`, `read`, `write`, `close`).
- **Important Functions:**
  - `pipe(int pipefd[2])`: Allocates a unidirectional pipe and populates two file descriptors: `pipefd[0]` (read) and `pipefd[1]` (write).
  - `write(int fd, const void *buf, size_t count)`: Writes data to a file descriptor.
  - `read(int fd, void *buf, size_t count)`: Reads data from a file descriptor into user memory.
  - `close(int fd)`: Deallocates a file descriptor.
- **How It Works:**
  1. `pipe(pipefd)` creates an internal circular buffer in the Linux kernel.
  2. The program writes a string into `pipefd[1]`.
  3. The write descriptor is closed.
  4. The program reads the data back into a buffer using `pipefd[0]`.
  5. The read descriptor is closed, freeing the kernel pipe resources.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic class_work/pipes/basic_pipe.c -o build/class_work/pipes/basic_pipe
  ```
- **Execution Command:**
  ```bash
  ./build/class_work/pipes/basic_pipe
  ```
- **Expected Output:**
  ```text
  ====================================================
         Inter-Process Communication: basic_pipe()    
  ====================================================

  [1] Creating kernel pipe with pipe()...
      Pipe created successfully:
      pipefd[0] (Read End File Descriptor)  : 3
      pipefd[1] (Write End File Descriptor) : 4

  [2] Writing message to pipefd[1]...
      Outgoing Content : "Operating Systems: Pipeline IPC in Action!" (43 bytes)
      Bytes written    : 43

  [3] Closing write descriptor pipefd[1]...
  [4] Reading data from pipefd[0]...
      Bytes read       : 43
      Received Content : "Operating Systems: Pipeline IPC in Action!"

  [5] Closing read descriptor pipefd[0]...
  ====================================================
    Pipe communication completed and cleaned up.
  ====================================================
  ```
- **Common Errors:**
  - Inverting descriptors: attempting to read from `pipefd[1]` or write to `pipefd[0]` returns `EBADF` (Bad file descriptor).
  - Leaving file descriptors unclosed, leading to descriptor leaks in long-running applications.

---

## 2. parent_child_pipe.c

- **Program Name:** `parent_child_pipe.c`
- **OS Concept:** Inter-Process Communication Across Processes, File Descriptor Inheritance, Deadlock Prevention.
- **Objective:** Establish a communication channel between a parent and child process across `fork()`, enforce the rule of closing unused descriptor ends, and synchronize completion with `waitpid()`.
- **Required Headers:**
  - `<stdio.h>`
  - `<stdlib.h>`
  - `<string.h>`
  - `<unistd.h>`
  - `<sys/types.h>`
  - `<sys/wait.h>`
- **Important Functions:**
  - `pipe()`: Initializes channel before `fork()`.
  - `fork()`: Duplicates process; child inherits clones of the pipe's open file descriptors.
  - `close()`: Closes unused ends (parent closes read end; child closes write end).
  - `waitpid()`: Parent reaps the child process upon completion.
- **How It Works:**
  1. The parent calls `pipe(pipefd)` before `fork()`.
  2. The parent forks the child process.
  3. Both parent and child now hold open descriptors for `pipefd[0]` and `pipefd[1]`.
  4. The child closes `pipefd[1]` (its unused write end) and blocks on `read(pipefd[0])`.
  5. The parent closes `pipefd[0]` (its unused read end) and writes the greeting to `pipefd[1]`.
  6. The parent closes `pipefd[1]`, causing an End-Of-File (EOF) condition to be signaled to the child.
  7. The child receives the message, prints it, and exits. The parent reaps the child.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic class_work/pipes/parent_child_pipe.c -o build/class_work/pipes/parent_child_pipe
  ```
- **Execution Command:**
  ./build/class_work/pipes/parent_child_pipe
  ```
- **Expected Output:**
  ```text
  ====================================================
     Parent-Child Pipe IPC: parent_child_pipe.c       
  ====================================================

  [Parent (PID 11020)] Pipe initialized: read_fd=3, write_fd=4
  [Parent] Forking child process...

  [Parent] Closed unused read descriptor 3.
  [Parent] Transmitting payload to child: "Hello from Parent! Remember to close unused pipe descriptors."
    [Child (PID 11021)] Closed unused write descriptor 4.
    [Child] Waiting to receive message from parent...
    [Child] Successfully read 61 bytes!
    [Child] Message Content: "Hello from Parent! Remember to close unused pipe descriptors."
    [Child] Closed read descriptor. Exiting.
  [Parent] Wrote 61 bytes. Closing write descriptor 4 (sends EOF)...
  [Parent] Child process reaped successfully. Done.
  ```
- **Common Errors:**
  - Failing to close the unused write end in the child: if the child keeps a write descriptor open, `read()` will block indefinitely waiting for EOF even after the parent closes its write descriptor (Pipe Deadlock).
  - Attempting to write to a pipe whose read end has been closed by all processes, triggering a `SIGPIPE` signal and terminating the writer abruptly.

