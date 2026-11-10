# Linux C Compilation & Build Guide

This guide details compiler standards, compilation flags, library linking, and Makefile commands for building the OS C Programming Lab.

---

## 1. Compiler Toolchain & Prerequisites

### Linux (Ubuntu / Debian / Linux Mint)
Install the standard GNU C toolchain:
```bash
sudo apt update
sudo apt install -y build-essential gcc make manpages-dev
```

### Windows (via WSL2 - Recommended for Windows Users)
Because process and POSIX system calls (`fork`, `pipe`, `wait`, `mmap`, `shm_open`) are native to the Linux kernel:
1. Open PowerShell as Administrator and run:
   ```powershell
   wsl --install -d Ubuntu
   ```
2. Restart your computer if prompted.
3. In the Ubuntu terminal, install GCC and Make:
   ```bash
   sudo apt update && sudo apt install -y build-essential
   ```

---

## 2. Recommended GCC Compiler Flags

When building programs manually or through the Makefile, these flags are strictly applied:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE
```

### Flag Breakdown:
- `-std=c11`: Enforces ISO/IEC 9899:2011 standard C syntax.
- `-Wall`: Enables all common compiler warnings (uninitialized variables, unused parameters).
- `-Wextra`: Enables additional strict warning diagnostics.
- `-Wpedantic`: Enforces strict ISO C compliance and rejects non-standard compiler extensions.
- `-D_POSIX_C_SOURCE=200809L`: Exposes modern POSIX.1-2008 system call definitions (`waitpid`, `ftruncate`, etc.).
- `-D_DEFAULT_SOURCE`: Exposes BSD/System V and glibc convenience definitions.

---

## 3. Linking Special Libraries

### POSIX Threads (`pthreads`)
- **Flag to Use:** `-pthread` (both during compilation and linking).
- **Why `-pthread` is better than `-lpthread`:**
  `-pthread` tells the compiler to configure reentrant thread-safe internal macros (e.g. `_REENTRANT`) in addition to passing `-lpthread` to the linker.

```bash
# Example thread compilation
gcc -std=c11 -Wall -Wextra -Wpedantic -pthread practice/threads/pthread_create.c -o build/practice/threads/pthread_create
```

### POSIX Realtime Extensions (`librt`)
- **Flag to Use:** `-lrt`
- Required on Linux for `shm_open()` and `shm_unlink()`. Must be placed *after* the source file in the command line:

```bash
# Example shared memory compilation
gcc -std=c11 -Wall -Wextra -Wpedantic assignments/shared_memory/shared_memory_write.c -o build/assignments/shared_memory/shared_memory_write -lrt
```

---

## 4. Makefile Usage Reference

From the `OS-C-Programming-Lab/` root directory:

```bash
# Build the entire laboratory
make

# Build specific category subsets
make practice        # Builds Folder 01
make class_work      # Builds Folder 02
make assignments     # Builds Folder 03
make labs            # Builds Folder 04
make final_examples  # Builds Folder 06

# Validate preparation documentation
make prep            # Checks Folder 05

# Run automated smoke test suite
make test

# Clean all generated binaries and temporary files
make clean

# Display helpful target index
make help
```

---

## 5. Compiling and Running a Single Program

You do not need to build the entire suite to run an individual experiment:

```bash
# Example: Process fork
gcc -std=c11 -Wall -Wextra practice/process_creation/fork_example.c -o fork_example
./fork_example

# Example: Mutex counter
gcc -std=c11 -Wall -Wextra -pthread labs/mutex/mutex_counter.c -o mutex_counter
./mutex_counter
```

