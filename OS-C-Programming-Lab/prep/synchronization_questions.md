# Operating Systems Viva & Exam Prep: Synchronization & Deadlocks

## 1. What is a Race Condition?
A **race condition** occurs when multiple concurrent threads or processes access and manipulate shared data, and the final outcome depends on the unpredictable order and timing in which execution is interleaved by the CPU scheduler.

---

## 2. What is a Critical Section?
A **critical section** is a segment of code where shared resources (variables, files, hardware devices) are accessed and modified.

### Four Requirements for a Valid Critical Section Solution:
1. **Mutual Exclusion:** If process $P_i$ is executing in its critical section, no other process can execute in its critical section.
2. **Progress:** If no process is executing in its critical section and some processes wish to enter, only those processes not in their remainder section can participate in deciding who enters next, and this selection cannot be postponed indefinitely.
3. **Bounded Waiting:** A bound must exist on the number of times other processes are allowed to enter their critical sections after a process has made a request to enter and before that request is granted (prevents starvation).
4. **Architectural Independence:** No assumptions can be made regarding relative speeds of the processes or the number of CPUs.

---

## 3. Mutex vs Semaphore Comparison

| Feature | Mutex (`pthread_mutex_t`) | Semaphore (`sem_t`) |
|---|---|---|
| **Underlying Nature** | Locking mechanism | Signaling mechanism |
| **Ownership** | **Strict ownership:** Only the thread that locked the mutex can unlock it | **No ownership:** Any thread can signal (`sem_post()`) to wake another thread |
| **Capacity** | Binary only (`0` or `1`) | Can be Binary (`0`, `1`) or Counting ($N$ resources) |
| **Use Case** | Mutual exclusion around critical data | Resource pool throttling, thread coordination & signaling |
| **Priority Inversion** | Can be resolved using Priority Inheritance protocols | Cannot easily track priority inheritance due to lack of owner |

---

## 4. Classic Synchronization Problems

### 1. The Bounded-Buffer (Producer-Consumer) Problem
- **Scenario:** Producers generate items into a shared finite buffer; consumers remove items.
- **Prerequisites:**
  - Producer must not write to a full buffer.
  - Consumer must not read from an empty buffer.
  - Buffer pointers must be updated mutually exclusively.
- **Solution:** Two counting semaphores (`empty = N`, `full = 0`) and one binary mutex.

### 2. The Readers-Writers Problem
- **Scenario:** Multiple threads access shared database. Multiple readers can read simultaneously, but writers require exclusive access.
- **Solution:** Reader-Writer lock (`pthread_rwlock_t`) or semaphore tracking active reader count.

### 3. The Dining Philosophers Problem
- **Scenario:** 5 philosophers sit around a table with 5 chopsticks. Each philosopher needs both left and right chopsticks to eat.
- **Danger:** If each philosopher picks up their left chopstick at the exact same instant, circular wait occurs, causing permanent deadlock.

---

## 5. Deadlocks: The Four Coffman Conditions
A **deadlock** is a state where a set of processes are blocked because each process is holding a resource and waiting for another resource held by another process in the set.

Deadlock can arise **if and only if** all four conditions hold simultaneously:
1. **Mutual Exclusion:** At least one resource must be held in a non-shareable mode.
2. **Hold and Wait:** A process must be holding at least one resource and waiting to acquire additional resources held by other processes.
3. **No Preemption:** Resources cannot be forcibly confiscated from a process; they can only be released voluntarily by the holding process.
4. **Circular Wait:** A closed chain of processes $\{P_0, P_1, \dots, P_n\}$ exists such that $P_0$ waits for a resource held by $P_1$, $P_1$ waits for $P_2$, and $P_n$ waits for $P_0$.

---

## 6. Deadlock Handling Strategies
1. **Prevention:** Disallow at least one of the four Coffman conditions (e.g. impose total global resource ordering to eliminate circular wait).
2. **Avoidance:** Dynamically examine resource allocation state to ensure system never enters an unsafe state (e.g., **Dijkstra's Banker's Algorithm**).
3. **Detection and Recovery:** Periodically construct a Resource-Allocation Graph (Wait-For Graph), detect cycles, and abort processes or preempt resources to break deadlocks.
4. **Ignoration (Ostrich Algorithm):** Ignore the problem under the assumption that deadlocks occur rarely and prevention overhead outweighs the cost (approach taken by general-purpose OS kernels like Linux).

