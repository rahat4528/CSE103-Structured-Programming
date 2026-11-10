# OS C Programming Lab

[![Language: C11](https://img.shields.io/badge/Language-C11-00599C?logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Platform: Linux](https://img.shields.io/badge/Platform-Linux%20%7C%20POSIX-FCC624?logo=linux&logoColor=black)](https://www.kernel.org/)
[![Compiler: GCC](https://img.shields.io/badge/Compiler-GCC%2011%2B-brightgreen)](https://gcc.gnu.org/)
[![Build: Make](https://img.shields.io/badge/Build-GNU%20Make-informational)](https://www.gnu.org/software/make/)
[![Standard: POSIX.1-2008](https://img.shields.io/badge/Standard-POSIX.1--2008-red)](https://pubs.opengroup.org/onlinepubs/9699919799/)

A modular, beginner-friendly educational repository containing practical C programming examples for core **Operating Systems** and **Linux System Programming** concepts. Every program demonstrates an isolated operating system mechanism through a clean, working command-line interface.

---

## Table of Contents
1. [Project Overview & Learning Objectives](#project-overview--learning-objectives)
2. [Included Concepts Matrix](#included-concepts-matrix)
3. [Repository Directory Structure](#repository-directory-structure)
4. [Prerequisites & System Requirements](#prerequisites--system-requirements)
5. [Installation & Setup](#installation--setup)
6. [Building with GNU Make](#building-with-gnu-make)
7. [Running Individual Programs](#running-individual-programs)
8. [Automated Smoke Test Suite](#automated-smoke-test-suite)
9. [Linux Compatibility & Platform Notes](#linux-compatibility--platform-notes)
10. [Documentation Index](#documentation-index)
11. [Contributing Guidelines](#contributing-guidelines)
12. [Future Expansion Roadmap](#future-expansion-roadmap)

---

## Project Overview & Learning Objectives

This laboratory is engineered for computer science undergraduate students, self-taught systems programmers, and educators. Instead of confronting massive monolithic operating systems codebases, students learn fundamentals by inspecting, compiling, modifying, and executing concise C programs.

### Core Learning Objectives:
- **Process Mechanics:** Master process creation (`fork`), synchronization (`wait`, `waitpid`), address space isolation (Copy-on-Write), and preventing zombie/orphan states.
- **Concurrent Multithreading:** Understand POSIX threads (`pthreads`), thread routines, parameter passing, concurrent decomposition, and thread joining.
- **Memory Dynamics:** Safely allocate, reallocate, zero-initialize, and free heap memory with `malloc`, `calloc`, `realloc`, and `free`, defending against memory leaks and dangling pointers.
- **Inter-Process Communication:** Explore unidirectional kernel pipelines (`pipe`), handle file descriptor pairs, avoid deadlocks, and implement high-speed zero-copy POSIX shared memory (`shm_open`, `mmap`, `shm_unlink`).
- **File Subsystems:** Distinguish buffered standard C streams (`FILE*`) from raw unbuffered POSIX system call descriptors (`int fd`).
- **CPU Scheduling:** Simulate and evaluate scheduling algorithms (FCFS, Non-preemptive SJF, Round Robin) with Gantt charts and mathematical verification.
- **Thread Synchronization:** Diagnose race conditions, protect critical sections with `pthread_mutex_t`, throttle access with counting semaphores (`sem_t`), and solve the Bounded-Buffer Producer-Consumer problem.

---

## Included Concepts Matrix

| Topic | Directory | Executable / Source | Key System Calls & APIs |
|---|---|---|---|
| **Process Creation** | `practice/process_creation/` | `fork_example.c` | `fork()`, `getpid()`, `getppid()` |
| **Process Reaping** | `practice/process_creation/` | `fork_wait.c` | `fork()`, `wait()`, `WIFEXITED()`, `WEXITSTATUS()` |
| **Thread Creation** | `practice/threads/` | `pthread_create.c` | `pthread_create()`, `pthread_self()`, `pthread_join()` |
| **Thread Joining** | `practice/threads/` | `pthread_join.c` | `pthread_create()`, `pthread_join()`, `malloc()`, `free()` |
| **Dynamic Malloc** | `practice/memory_allocation/` | `malloc_example.c` | `malloc()`, `free()` |
| **Zeroed Calloc** | `practice/memory_allocation/` | `calloc_example.c` | `calloc()`, `free()` |
| **Dynamic Realloc** | `practice/memory_allocation/` | `realloc_example.c` | `realloc()`, `free()` |
| **Basic Pipe** | `class_work/pipes/` | `basic_pipe.c` | `pipe()`, `read()`, `write()`, `close()` |
| **Parent-Child Pipe**| `class_work/pipes/` | `parent_child_pipe.c` | `pipe()`, `fork()`, `read()`, `write()`, `waitpid()` |
| **File Reading** | `class_work/file_handling/` | `file_read.c` | `fopen("r")`, `fgets()`, `fclose()` |
| **File Writing** | `class_work/file_handling/` | `file_write.c` | `fopen("w")`, `fprintf()`, `fputs()`, `fclose()` |
| **File Appending** | `class_work/file_handling/` | `file_append.c` | `fopen("a")`, `fprintf()`, `fclose()` |
| **Shared Memory Write**| `assignments/shared_memory/` | `shared_memory_write.c` | `shm_open()`, `ftruncate()`, `mmap()`, `munmap()` |
| **Shared Memory Read** | `assignments/shared_memory/` | `shared_memory_read.c` | `shm_open()`, `mmap()`, `munmap()`, `shm_unlink()` |
| **FCFS Scheduling** | `assignments/cpu_scheduling/`| `fcfs.c` | Ready queue traversal, Gantt chart, metric calculations |
| **SJF Scheduling** | `assignments/cpu_scheduling/`| `sjf.c` | Non-preemptive sorting, minimal wait time verification |
| **Round Robin** | `assignments/cpu_scheduling/`| `round_robin.c` | Cyclic time quantum queue ($Q=4$), slice tracking |
| **Mutex Counter** | `labs/mutex/` | `mutex_counter.c` | `pthread_mutex_init()`, `pthread_mutex_lock()`, `unlock()`, `destroy()` |
| **Counting Semaphore**| `labs/semaphores/` | `semaphore_example.c` | `sem_init()`, `sem_wait()`, `sem_post()`, `sem_destroy()` |
| **Producer-Consumer**| `labs/semaphores/` | `producer_consumer.c` | Circular buffer, 2 semaphores (`empty`, `full`) + 1 mutex |
| **Combined Duplex IPC**| `final_examples/` | `process_communication.c` | Two pipes, `fork()`, calculation worker, structured streaming |
| **Thread-Safe Bank** | `final_examples/` | `thread_synchronization.c` | Concurrent deposits/withdrawals, invariant verification |
| **Dynamic Vector** | `final_examples/` | `memory_management.c` | Geometric buffer growth, defensive realloc, zeroed base |

---

## Repository Directory Structure

```text
OS-C-Programming-Lab/
│
├── README.md                           # Master repository documentation
├── CHEATSHEET.md                       # High-yield quick-reference cheatsheet
├── Makefile                            # Master build orchestration system
├── .gitignore                          # Build artifacts and temp file exclusion rules
│
├── practice/                           # FOLDER 01: Introductory Fundamentals
│   ├── process_creation/
│   │   ├── fork_example.c              # Basic fork and memory isolation demo
│   │   ├── fork_wait.c                 # wait() and exit code reaping
│   │   └── README.md
│   ├── threads/
│   │   ├── pthread_create.c            # Single thread spawn and parameter passing
│   │   ├── pthread_join.c              # Multi-thread concurrent map-reduce sum
│   │   └── README.md
│   └── memory_allocation/
│       ├── malloc_example.c            # Heap allocation with null check
│       ├── calloc_example.c            # Zero-initialized contiguous allocation
│       ├── realloc_example.c           # Defensive buffer resizing
│       └── README.md
│
├── class_work/                         # FOLDER 02: Classroom Standard Exercises
│   ├── pipes/
│   │   ├── basic_pipe.c                # Single-process pipe read/write
│   │   ├── parent_child_pipe.c         # Cross-process pipeline communication
│   │   └── README.md
│   └── file_handling/
│       ├── file_read.c                 # Line-by-line buffered file reading
│       ├── file_write.c                # Formatted stream creation
│       ├── file_append.c               # Non-destructive log appending
│       ├── sample_input.txt            # Ready-to-use sample text file
│       └── README.md
│
├── assignments/                        # FOLDER 03: Advanced OS Assignments
│   ├── shared_memory/
│   │   ├── shared_memory_write.c       # POSIX shm creation and serialization
│   │   ├── shared_memory_read.c        # Ingestion and kernel shm_unlink cleanup
│   │   └── README.md
│   └── cpu_scheduling/
│       ├── fcfs.c                      # First-Come First-Served simulation
│       ├── sjf.c                       # Shortest Job First simulation
│       ├── round_robin.c               # Round Robin time-sliced simulation
│       └── README.md
│
├── labs/                               # FOLDER 04: Synchronization Laboratories
│   ├── mutex/
│   │   ├── mutex_counter.c             # Race condition manifestation vs mutex fix
│   │   └── README.md
│   └── semaphores/
│       ├── semaphore_example.c         # POSIX counting semaphore throttling
│       ├── producer_consumer.c         # Bounded-buffer producer-consumer solution
│       └── README.md
│
├── prep/                               # FOLDER 05: Exam & Viva Revision Guides
│   ├── process_questions.md            # Process theory, states, PCB, fork/exec
│   ├── thread_questions.md             # Thread architectures, pthreads, concurrency
│   ├── memory_questions.md             # Virtual memory segments, heap vs stack
│   ├── synchronization_questions.md    # Critical sections, mutex vs sem, deadlocks
│   ├── scheduling_questions.md         # Scheduling criteria, algorithm trade-offs
│   └── viva_revision.md                # Rapid-fire Q&A, pitfalls, system call tips
│
├── final_examples/                     # FOLDER 06: Comprehensive Integrations
│   ├── process_communication.c         # Bidirectional duplex pipe IPC
│   ├── thread_synchronization.c        # Multi-worker financial invariant audit
│   ├── memory_management.c             # Dynamic vector data structure lifecycle
│   └── README.md
│
├── docs/                               # Detailed Technical Guides
│   ├── concepts.md                     # Deep theoretical operating systems manual
│   ├── system_calls.md                 # Complete system call signatures & errors
│   ├── compilation_guide.md            # GCC flags, pthread linking, WSL setup
│   └── troubleshooting.md              # Common Linux errors and diagnostics
│
├── scripts/
│   └── smoke_test.sh                   # Automated end-to-end test validation suite
│
└── build/                              # Compiled output executables (git-ignored)
    └── .gitkeep
```

---

## Prerequisites & System Requirements

- **Operating System:** Linux (Ubuntu 20.04+, Debian 11+, Arch, Fedora, RHEL) or Windows WSL2.
- **Compiler:** GCC 7.0+ (supporting `-std=c11`).
- **Build Utility:** GNU Make 4.0+.
- **Standard Libraries:** `glibc` with POSIX threads (`libpthread`) and realtime extensions (`librt`).

### Installing on Ubuntu / Debian:
```bash
sudo apt update
sudo apt install -y build-essential gcc make manpages-dev
```

### Installing on Fedora / RHEL:
```bash
sudo dnf groupinstall "Development Tools"
```

---

## Installation & Setup

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/rahat4528/CSE103-Structured-Programming.git
   cd CSE103-Structured-Programming/OS-C-Programming-Lab
   ```
2. **Verify Toolchain:**
   ```bash
   gcc --version
   make --version
   ```

---

## Building with GNU Make

The repository includes a root `Makefile` configured with strict compiler flags:
`-std=c11 -Wall -Wextra -Wpedantic -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE`

```bash
# 1. Compile all programs across the entire repository
make

# 2. Compile specific topic categories
make practice        # Builds Folder 01
make class_work      # Builds Folder 02
make assignments     # Builds Folder 03
make labs            # Builds Folder 04
make final_examples  # Builds Folder 06

# 3. Verify all preparation and viva Markdown documents
make prep

# 4. Remove all compiled executables and test files
make clean

# 5. Display help menu with all available targets
make help
```

---

## Running Individual Programs

Executables are placed into the `build/` directory preserving the source hierarchy:

```bash
# --- Process Creation ---
./build/practice/process_creation/fork_example
./build/practice/process_creation/fork_wait

# --- Threads ---
./build/practice/threads/pthread_create
./build/practice/threads/pthread_join

# --- Dynamic Memory ---
./build/practice/memory_allocation/malloc_example
./build/practice/memory_allocation/calloc_example
./build/practice/memory_allocation/realloc_example

# --- Pipes & File Handling ---
./build/class_work/pipes/basic_pipe
./build/class_work/pipes/parent_child_pipe
./build/class_work/file_handling/file_write
./build/class_work/file_handling/file_append
./build/class_work/file_handling/file_read

# --- Shared Memory (Run writer first, then reader!) ---
./build/assignments/shared_memory/shared_memory_write
./build/assignments/shared_memory/shared_memory_read

# --- CPU Scheduling Simulations ---
./build/assignments/cpu_scheduling/fcfs
./build/assignments/cpu_scheduling/sjf
./build/assignments/cpu_scheduling/round_robin

# --- Synchronization & Semaphores ---
./build/labs/mutex/mutex_counter
./build/labs/semaphores/semaphore_example
./build/labs/semaphores/producer_consumer

# --- Final Integrations ---
./build/final_examples/process_communication
./build/final_examples/thread_synchronization
./build/final_examples/memory_management
```

---

## Automated Smoke Test Suite

To verify that all programs compile cleanly, link with their respective libraries, and execute without segfaults or memory errors:

```bash
make test
# OR directly:
chmod +x scripts/smoke_test.sh
./scripts/smoke_test.sh
```

The smoke test runs every binary in non-interactive verification mode, tests inter-process communications, validates scheduling arithmetic, checks POSIX shared memory creation and unlinking, and outputs a colored summary report.

---

## Linux Compatibility & Platform Notes

- **Native Linux:** Fully compatible out of the box with any modern Linux distribution.
- **Windows (WSL2):** Recommended for Windows users. WSL2 runs a genuine Linux kernel, offering native support for POSIX system calls (`fork`, `wait`, `pipe`, `shm_open`, `mmap`).
- **Windows MinGW/MSYS2:** Standard C programs (dynamic memory, file handling, CPU scheduling) compile under MinGW. However, POSIX process calls (`fork()`, POSIX unnamed semaphores, `shm_open`) are POSIX-specific and require WSL2 or a Linux environment.
- **macOS:** BSD-based macOS supports `fork()`, `pipe()`, `pthreads`, and `mmap()`. Note that macOS has deprecated unnamed semaphores (`sem_init()`) in favor of named semaphores (`sem_open()`). For standard POSIX unnamed semaphore testing, Linux is the target environment.

---

## Documentation Index

- [`CHEATSHEET.md`](./CHEATSHEET.md) — Quick reference for commands, formulas, and common compilation flags.
- [`docs/concepts.md`](./docs/concepts.md) — Theoretical guide covering OS architecture, memory layouts, IPC, and scheduling.
- [`docs/system_calls.md`](./docs/system_calls.md) — Detailed reference for system calls used in this repository.
- [`docs/compilation_guide.md`](./docs/compilation_guide.md) — GCC flag reference, pthread linking guide, and WSL setup instructions.
- [`docs/troubleshooting.md`](./docs/troubleshooting.md) — Debugging pipe deadlocks, stale shared memory, and segmentation faults.
- [`prep/`](./prep/) — Dedicated exam and viva preparation guides covering processes, threads, memory, synchronization, and scheduling.

---

## Contributing Guidelines

Contributions that enhance educational clarity are welcome!
1. **Fork the repository** and create a feature branch (`git checkout -b feature/topic-example`).
2. **Adhere to Code Quality:** Keep programs focused, concise, and independently compilable in C11 with `-Wall -Wextra -Wpedantic`.
3. **Include Documentation:** Update topic READMEs and follow the standardized section format.
4. **Test Thoroughly:** Ensure `scripts/smoke_test.sh` passes completely before submitting a pull request.

---

## Future Expansion Roadmap

- [ ] Add Banker's Algorithm deadlock avoidance simulation.
- [ ] Add Reader-Writer lock demonstration (`pthread_rwlock_t`).
- [ ] Add UNIX Domain Sockets (`AF_UNIX`) inter-process communication example.
- [ ] Add POSIX Message Queues (`mq_open`, `mq_send`, `mq_receive`).
- [ ] Add Memory-Mapped File I/O (`mmap` with regular files).
- [ ] Add Signal handling and asynchronous event processing (`sigaction`).

---

## Author & Academic Affiliation

* **Mahamudul Hasan Rahat**
  * GitHub: [@rahat4528](https://github.com/rahat4528)
  * Course: **CSE103 — Structured Programming Language / Operating Systems Lab**

