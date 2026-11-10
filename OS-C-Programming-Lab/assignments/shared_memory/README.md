# POSIX Shared Memory Assignments

This folder demonstrates high-performance inter-process communication using Linux POSIX shared memory (`shm_open`, `ftruncate`, `mmap`, `munmap`, and `shm_unlink`), which maps pages directly into virtual memory without kernel copy overhead.

---

## Execution Order and Lifecycle

POSIX shared memory objects persist in the kernel (`/dev/shm`) until explicitly unlinked or until system reboot.

### Correct Sequence:
1. **Run Writer:** `./build/assignments/shared_memory/shared_memory_write`
   - Creates the `/dev/shm/os_c_shm_demo` memory-mapped file.
   - Sizes the segment to accommodate the `SharedData` struct.
   - Populates the shared data fields.
2. **Run Reader:** `./build/assignments/shared_memory/shared_memory_read`
   - Opens the existing object.
   - Reads and validates the contents.
   - Calls `shm_unlink("/os_c_shm_demo")` to cleanly delete the object from `/dev/shm`.

> [!IMPORTANT]
> If `shared_memory_read` is run before `shared_memory_write`, `shm_open()` returns `ENOENT` (No such file or directory).
> If `shared_memory_read` fails to unlink the object, stale data persists in `/dev/shm` and can be manually cleared with:
> `rm -f /dev/shm/os_c_shm_demo`

---

## 1. shared_memory_write.c

- **Program Name:** `shared_memory_write.c`
- **OS Concept:** POSIX Shared Memory Creation, Memory Mapping (`mmap`), Zero-Copy IPC.
- **Objective:** Create a shared memory object using `shm_open()`, set size with `ftruncate()`, map it with `mmap()`, serialize structured payload, and unmap safely.
- **Required Headers:**
  - `<stdio.h>`
  - `<stdlib.h>`
  - `<string.h>`
  - `<fcntl.h>`: File control options (`O_CREAT`, `O_RDWR`).
  - `<unistd.h>`: POSIX calls (`ftruncate`, `close`, `getpid`).
  - `<sys/mman.h>`: Memory management (`shm_open`, `mmap`, `munmap`, `shm_unlink`).
  - `<sys/stat.h>`: Mode flags.
- **Important Functions:**
  - `shm_open(name, oflag, mode)`: Establishes a connection between a shared memory object and a file descriptor.
  - `ftruncate(fd, length)`: Sets the byte size of the shared memory region.
  - `mmap(addr, length, prot, flags, fd, offset)`: Maps the shared memory file descriptor into process address space.
  - `munmap(addr, length)`: Unmaps previously mapped memory region.
- **How It Works:**
  1. Calls `shm_open("/os_c_shm_demo", O_CREAT | O_RDWR, 0666)`.
  2. Sets size to `sizeof(SharedData)` via `ftruncate()`.
  3. Maps buffer into virtual address space via `mmap()`.
  4. Writes PID, message ID, and string to shared memory.
  5. Unmaps virtual pointer and closes the descriptor.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic assignments/shared_memory/shared_memory_write.c -o build/assignments/shared_memory/shared_memory_write -lrt
  ```
- **Execution Command:**
  ```bash
  ./build/assignments/shared_memory/shared_memory_write
  ```
- **Expected Output:**
  ```text
  ====================================================
         POSIX Shared Memory Writer: shm_open()       
  ====================================================

  [1] Creating / opening shared memory object: "/os_c_shm_demo"...
      Shared memory object opened. Descriptor: 3

  [2] Setting shared memory size to 136 bytes with ftruncate()...
      Memory segment sized successfully.

  [3] Mapping memory segment into process address space with mmap()...
      Memory mapped at virtual address: 0x7fa24d100000

  [4] Writing structured data into shared memory...
      Sender PID : 12450
      Message ID : 101
      Payload    : "Inter-Process Communication via Linux POSIX Shared Memory!"

  [5] Synchronizing and unmapping with munmap()...
      File descriptor closed. Shared memory object remains resident in /dev/shm.

  >>> NEXT STEP: Run 'shared_memory_read' to inspect and unlink this object.
  ====================================================
  ```
- **Common Errors:**
  - Forgetting `-lrt` flag on older Linux distributions (`undefined reference to shm_open`).
  - Omitting `ftruncate()`: newly created shared memory objects have size 0; accessing mapped memory results in `SIGBUS` (Bus error).

---

## 2. shared_memory_read.c

- **Program Name:** `shared_memory_read.c`
- **OS Concept:** POSIX Shared Memory Ingestion, Read-Only Memory Protection, Object Deletion via `shm_unlink`.
- **Objective:** Open the pre-existing shared memory object, map as read-only (`PROT_READ`), read and display payload, and unlink the object to prevent stale artifacts.
- **Required Headers:**
  - `<stdio.h>`
  - `<stdlib.h>`
  - `<fcntl.h>`
  - `<unistd.h>`
  - `<sys/mman.h>`
  - `<sys/stat.h>`
- **Important Functions:**
  - `shm_open(name, O_RDONLY, ...)`: Connects to existing object.
  - `mmap(..., PROT_READ, MAP_SHARED, ...)`: Maps as read-only.
  - `munmap()`: Unmaps segment from caller address space.
  - `shm_unlink(name)`: Removes shared memory object name from `/dev/shm`.
- **How It Works:**
  1. Opens `/os_c_shm_demo`.
  2. Maps segment with `PROT_READ`.
  3. Reads stored message and metadata.
  4. Unmaps memory and closes file descriptor.
  5. Calls `shm_unlink()` to cleanly remove object from Linux kernel.
- **Compilation Command:**
  ```bash
  gcc -std=c11 -Wall -Wextra -Wpedantic assignments/shared_memory/shared_memory_read.c -o build/assignments/shared_memory/shared_memory_read -lrt
  ```
- **Execution Command:**
  ```bash
  ./build/assignments/shared_memory/shared_memory_read
  ```
- **Expected Output:**
  ```text
  ====================================================
          POSIX Shared Memory Reader: mmap()          
  ====================================================

  [1] Opening existing shared memory object: "/os_c_shm_demo"...
      Shared memory object located. Descriptor: 3

  [2] Mapping shared memory segment as read-only via mmap()...
      Mapped at virtual address: 0x7fa24d100000

  [3] Reading payload from shared memory:
  ---------------- SHARED MEMORY DATA ----------------
    Original Sender PID : 12450
    Message Sequence ID : 101
    Transmitted Text    : "Inter-Process Communication via Linux POSIX Shared Memory!"
  ----------------------------------------------------

  [4] Unmapping memory with munmap()...
  [5] Cleaning up shared memory object with shm_unlink()...
      Shared memory object "/os_c_shm_demo" removed from /dev/shm.
      No stale shared memory remains in the operating system.
  ====================================================
  ```
- **Common Errors:**
  - Calling `shm_unlink()` too early before reader runs, causing `shm_open()` in the reader to fail with `ENOENT`.
  - Writing to memory mapped with `PROT_READ`, triggering `SIGSEGV` (Segmentation fault).

