#!/usr/bin/env bash
# ==============================================================================
# OS C Programming Lab - Automated Smoke Test Suite
# Target: Linux / POSIX Shell (bash)
# ==============================================================================

set -u

# Terminal Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
BOLD='\033[1m'
NC='\033[0m' # No Color

TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

BUILD_DIR="build"

echo -e "${BOLD}${BLUE}============================================================${NC}"
echo -e "${BOLD}${BLUE}       OS C Programming Lab: Automated Smoke Test Suite     ${NC}"
echo -e "${BOLD}${BLUE}============================================================${NC}"
echo ""

# Ensure we are in project root
if [ ! -f "Makefile" ]; then
    if [ -f "../Makefile" ]; then
        cd ..
    fi
fi

# Ensure clean build
echo -e "${YELLOW}[Step 1] Building all targets via Makefile...${NC}"
make clean > /dev/null 2>&1
if ! make all > /dev/null 2>&1; then
    echo -e "${RED}[FATAL] Compilation failed during 'make all'!${NC}"
    exit 1
fi
echo -e "${GREEN}[OK] All programs compiled successfully.${NC}"
echo ""

# Helper test runner: checks exit code 0 and optional expected substring
run_test() {
    local name="$1"
    local bin="$2"
    local expected="${3:-}"
    local args="${4:-}"

    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -n "  Testing: ${name}... "

    if [ ! -x "$bin" ]; then
        echo -e "${RED}[FAILED] Executable '$bin' not found or not executable!${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi

    local output
    if ! output=$("$bin" $args 2>&1); then
        echo -e "${RED}[FAILED] (Exit code: $?)${NC}"
        echo -e "    Error output:\n${output}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi

    if [ -n "$expected" ] && ! printf '%s\n' "$output" | grep -Fq -- "$expected"; then
        echo -e "${RED}[FAILED] (Expected output missing: '$expected')${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi

    echo -e "${GREEN}[PASSED]${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
    return 0
}

# Helper negative test runner: checks that invalid input exits non-zero
run_expect_fail() {
    local name="$1"
    local bin="$2"
    local stdin_data="${3:-}"
    local args="${4:-}"

    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -n "  Testing (negative): ${name}... "

    if [ ! -x "$bin" ]; then
        echo -e "${RED}[FAILED] Executable '$bin' not found!${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi

    local output
    if output=$(printf '%b' "$stdin_data" | "$bin" $args 2>&1); then
        echo -e "${RED}[FAILED] (Expected non-zero exit code, got 0)${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi

    echo -e "${GREEN}[PASSED]${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
    return 0
}

echo -e "${YELLOW}[Step 2] Executing Category 01: Practice Tests...${NC}"
run_test "Process Fork" "$BUILD_DIR/practice/process_creation/fork_example" "Parent sees shared_variable = 100"
run_test "Process Wait & Reap" "$BUILD_DIR/practice/process_creation/fork_wait" "Reaped Exit Code : 42"
run_test "Thread Create" "$BUILD_DIR/practice/threads/pthread_create" "Task completed successfully."
run_test "Thread Join Concurrency" "$BUILD_DIR/practice/threads/pthread_join" "Combined Total Sum of 1..100 : 5050"
run_test "Dynamic Malloc" "$BUILD_DIR/practice/memory_allocation/malloc_example" "Computed sum of array elements: 55"
run_test "Zeroed Calloc" "$BUILD_DIR/practice/memory_allocation/calloc_example" "All 6 elements were automatically initialized to 0"
run_test "Dynamic Realloc" "$BUILD_DIR/practice/memory_allocation/realloc_example" "scores[5] = 100"
echo ""

echo -e "${YELLOW}[Step 3] Executing Category 02: Class Work Tests...${NC}"
run_test "Basic Pipe" "$BUILD_DIR/class_work/pipes/basic_pipe" "Operating Systems: Pipeline IPC in Action!"
run_test "Parent-Child Pipe" "$BUILD_DIR/class_work/pipes/parent_child_pipe" "Hello from Parent!"
run_test "File Stream Write" "$BUILD_DIR/class_work/file_handling/file_write" "File stream flushed to disk and closed."
run_test "File Stream Append" "$BUILD_DIR/class_work/file_handling/file_append" "Appending new OS record"
run_test "File Stream Read" "$BUILD_DIR/class_work/file_handling/file_read" "Operating Systems Laboratory"
echo ""

echo -e "${YELLOW}[Step 4] Executing Category 03: Assignments Tests...${NC}"
rm -f /dev/shm/os_c_shm_demo
run_expect_fail "Shared Memory Read Before Write" "$BUILD_DIR/assignments/shared_memory/shared_memory_read"
run_test "Shared Memory Write" "$BUILD_DIR/assignments/shared_memory/shared_memory_write" "Shared memory object remains resident"
run_test "Shared Memory Read & Unlink" "$BUILD_DIR/assignments/shared_memory/shared_memory_read" "Inter-Process Communication via Linux POSIX Shared Memory!"
rm -f /dev/shm/os_c_shm_demo

run_test "FCFS Scheduling" "$BUILD_DIR/assignments/cpu_scheduling/fcfs" "Average Waiting Time    : 10.25 ms"
run_test "SJF Scheduling" "$BUILD_DIR/assignments/cpu_scheduling/sjf" "Average Waiting Time    : 7.00 ms"
run_test "Round Robin Scheduling" "$BUILD_DIR/assignments/cpu_scheduling/round_robin" "Average Waiting Time    : 13.25 ms"
echo ""

echo -e "${YELLOW}[Step 5] Executing Category 04: Labs Tests...${NC}"
run_test "Mutex Counter Sync" "$BUILD_DIR/labs/mutex/mutex_counter" "Actual Count   : 500000"
run_test "Counting Semaphore" "$BUILD_DIR/labs/semaphores/semaphore_example" "Semaphore destroyed successfully."
run_test "Producer-Consumer Queue" "$BUILD_DIR/labs/semaphores/producer_consumer" "All 10 items successfully produced and consumed."
echo ""

echo -e "${YELLOW}[Step 6] Executing Category 06: Final Combined Examples...${NC}"
run_test "Bidirectional Duplex Pipe IPC" "$BUILD_DIR/final_examples/process_communication" "Sum     : 239"
run_test "Thread Synchronization Bank" "$BUILD_DIR/final_examples/thread_synchronization" "Actual Final Balance    : \$25000"
run_test "Dynamic Vector Lifecycle" "$BUILD_DIR/final_examples/memory_management" "[ 10, 25, 40, 55, 70, 85 ]"
echo ""

echo -e "${YELLOW}[Step 7] Executing Invalid-Input & Error-Handling Tests...${NC}"
run_expect_fail "File Read Missing Path" "$BUILD_DIR/class_work/file_handling/file_read" "" "nonexistent_file_12345.txt"
run_expect_fail "FCFS Invalid CLI Flag" "$BUILD_DIR/assignments/cpu_scheduling/fcfs" "" "--invalid-flag"
run_expect_fail "FCFS Invalid Process Count" "$BUILD_DIR/assignments/cpu_scheduling/fcfs" "-2\n" "--custom"
run_expect_fail "SJF Invalid Burst Time" "$BUILD_DIR/assignments/cpu_scheduling/sjf" "1\n0\n" "--custom"
run_expect_fail "Round Robin Invalid Quantum" "$BUILD_DIR/assignments/cpu_scheduling/round_robin" "1\n5\n0\n" "--custom"
echo ""

# Cleanup runtime generated demo output
rm -f class_work/file_handling/demo_output.txt

echo -e "${BOLD}${BLUE}============================================================${NC}"
echo -e "${BOLD}                       TEST SUMMARY                         ${NC}"
echo -e "${BOLD}${BLUE}============================================================${NC}"
echo -e "  Total Tests Executed : ${TOTAL_TESTS}"
echo -e "  Tests Passed         : ${GREEN}${PASSED_TESTS}${NC}"
echo -e "  Tests Failed         : ${RED}${FAILED_TESTS}${NC}"

if [ "$FAILED_TESTS" -eq 0 ]; then
    echo -e "${BOLD}${GREEN}>>> ALL TESTS PASSED! Operating Systems Repository is verified. <<<${NC}"
    exit 0
else
    echo -e "${BOLD}${RED}>>> SOME TESTS FAILED! Check outputs above. <<<${NC}"
    exit 1
fi
