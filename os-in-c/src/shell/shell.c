/**
 * CSE103-OS: Interactive Kernel Command Shell
 * Unified command interpreter reading input from both PS/2 keyboard and COM1 serial,
 * with line editing, argument parsing, and an extensible command table.
 */

#include "shell.h"
#include "keyboard.h"
#include "serial.h"
#include "vga.h"
#include "kprintf.h"
#include "string.h"
#include "timer.h"
#include "pmm.h"
#include "heap.h"
#include "fs.h"
#include "context.h"
#include "io.h"

static char line_buf[SHELL_MAX_CMD_LEN];
static size_t line_len = 0;

static void cmd_help(int argc, char *argv[]);
static void cmd_clear(int argc, char *argv[]);
static void cmd_echo(int argc, char *argv[]);
static void cmd_uptime(int argc, char *argv[]);
static void cmd_meminfo(int argc, char *argv[]);
static void cmd_reboot(int argc, char *argv[]);
static void cmd_halt(int argc, char *argv[]);
static void cmd_tasks(int argc, char *argv[]);
static void cmd_ls(int argc, char *argv[]);
static void cmd_cat(int argc, char *argv[]);
static void cmd_test(int argc, char *argv[]);

static const shell_command_t command_table[] = {
    {"help",    "Display list of available commands", cmd_help},
    {"clear",   "Clear VGA terminal screen", cmd_clear},
    {"echo",    "Print string arguments to output", cmd_echo},
    {"uptime",  "Show system running time in seconds", cmd_uptime},
    {"meminfo", "Show physical memory and kernel heap usage", cmd_meminfo},
    {"reboot",  "Reset computer via keyboard controller", cmd_reboot},
    {"halt",    "Halt the CPU", cmd_halt},
    {"tasks",   "Display multitasking threads", cmd_tasks},
    {"ls",      "List files in RAM-disk file system", cmd_ls},
    {"cat",     "Display file content: cat <filename>", cmd_cat},
    {"test",    "Run kernel self-tests (PMM, Heap, Ramdisk)", cmd_test},
    {NULL, NULL, NULL}
};

static void print_prompt(void) {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    kprintf("CSE103-OS> ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

static void cmd_help(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    kprintf("Available CSE103-OS Shell Commands:\n");
    for (int i = 0; command_table[i].name != NULL; i++) {
        kprintf("  %-10s - %s\n", command_table[i].name, command_table[i].description);
    }
}

static void cmd_clear(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    vga_clear();
}

static void cmd_echo(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        kprintf("%s%s", argv[i], (i + 1 < argc) ? " " : "");
    }
    kprintf("\n");
}

static void cmd_uptime(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    uint32_t sec = timer_get_uptime_sec();
    uint32_t ticks = timer_get_ticks();
    kprintf("Uptime: %u seconds (%u ticks at 100 Hz)\n", sec, ticks);
}

static void cmd_meminfo(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    uint32_t total_kb = pmm_get_total_memory_kb();
    uint32_t total_f  = pmm_get_total_frames();
    uint32_t used_f   = pmm_get_used_frames();
    uint32_t free_f   = pmm_get_free_frames();

    kprintf("=== Physical Memory Manager (PMM) ===\n");
    kprintf("Total RAM:   %u KB (%u MB)\n", total_kb, total_kb / 1024);
    kprintf("Total 4KB Frames: %u\n", total_f);
    kprintf("Used Frames:      %u (%u KB)\n", used_f, used_f * 4);
    kprintf("Free Frames:      %u (%u KB)\n", free_f, free_f * 4);

    kprintf("\n=== Kernel Dynamic Heap (kmalloc) ===\n");
    kprintf("Heap Base:      0x%x (Size: %u KB)\n", KERNEL_HEAP_START, KERNEL_HEAP_SIZE / 1024);
    kprintf("Allocated:      %u bytes\n", (uint32_t)heap_get_allocated_bytes());
    kprintf("Free:           %u bytes\n", (uint32_t)heap_get_free_bytes());
}

static void cmd_reboot(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    kprintf("Rebooting CSE103-OS...\n");
    serial_puts("[KERNEL] System reboot initiated.\n");

    /*
     * Pulse the CPU reset line using the 8042 PS/2 keyboard controller.
     * Sending 0xFE to port 0x64 asserts the system RESET signal.
     */
    uint8_t temp;
    do {
        temp = inb(0x64);
        if (temp & 1) inb(0x60);
    } while (temp & 2);
    outb(0x64, 0xFE);

    while (1) {
        hlt();
    }
}

static void cmd_halt(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    kprintf("Halting CSE103-OS CPU. Goodbye.\n");
    serial_puts("[KERNEL] CPU halted via shell command.\n");
    cli();
    while (1) {
        hlt();
    }
}

static void cmd_tasks(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    task_list();
}

static void cmd_ls(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    fs_list();
}

static void cmd_cat(int argc, char *argv[]) {
    if (argc < 2) {
        kprintf("Usage: cat <filename>\n");
        return;
    }
    size_t size = 0;
    const char *content = fs_read(argv[1], &size);
    if (!content) {
        kprintf("cat: file not found: '%s'\n", argv[1]);
        return;
    }
    kprintf("%s\n", content);
}

static void cmd_test(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    kprintf("--- Running CSE103-OS Kernel Verification Tests ---\n");

    /* Test 1: PMM frame allocation */
    void *frame1 = pmm_alloc_frame();
    void *frame2 = pmm_alloc_frame();
    if (frame1 && frame2 && frame1 != frame2) {
        kprintf("[PASS] PMM frame allocator: allocated frames at 0x%x and 0x%x\n",
                (uint32_t)frame1, (uint32_t)frame2);
        pmm_free_frame(frame1);
        pmm_free_frame(frame2);
        kprintf("[PASS] PMM frame allocator: freed frames cleanly\n");
    } else {
        kprintf("[FAIL] PMM frame allocation failed!\n");
    }

    /* Test 2: Kernel dynamic heap */
    char *buf = (char *)kmalloc(128);
    if (buf) {
        strcpy(buf, "CSE103 Heap Allocation Test String");
        if (strcmp(buf, "CSE103 Heap Allocation Test String") == 0) {
            kprintf("[PASS] Kernel heap: kmalloc and string validation passed\n");
        } else {
            kprintf("[FAIL] Kernel heap memory content corrupted!\n");
        }
        kfree(buf);
        kprintf("[PASS] Kernel heap: kfree executed successfully\n");
    } else {
        kprintf("[FAIL] Kernel heap: kmalloc returned NULL!\n");
    }

    /* Test 3: Ramdisk file system */
    size_t s = 0;
    const char *readme = fs_read("/readme.txt", &s);
    if (readme && s > 0) {
        kprintf("[PASS] RAM disk: read /readme.txt (%u bytes)\n", (uint32_t)s);
    } else {
        kprintf("[FAIL] RAM disk: failed to read /readme.txt!\n");
    }

    kprintf("--- Verification Complete ---\n");
}

void shell_execute(char *cmd_line) {
    while (*cmd_line == ' ') cmd_line++;
    if (*cmd_line == '\0') return;

    char *argv[SHELL_MAX_ARGS];
    int argc = 0;

    char *token = cmd_line;
    while (*token != '\0' && argc < SHELL_MAX_ARGS) {
        while (*token == ' ') {
            *token++ = '\0';
        }
        if (*token == '\0') break;

        argv[argc++] = token;
        while (*token != '\0' && *token != ' ') {
            token++;
        }
    }

    if (argc == 0) return;

    for (int i = 0; command_table[i].name != NULL; i++) {
        if (strcmp(argv[0], command_table[i].name) == 0) {
            command_table[i].func(argc, argv);
            return;
        }
    }

    kprintf("Unknown command: '%s'. Type 'help' for available commands.\n", argv[0]);
}

void shell_init(void) {
    line_len = 0;
    line_buf[0] = '\0';
    kprintf("\nType 'help' for commands or 'test' to run kernel diagnostics.\n");
    print_prompt();
}

void shell_update(void) {
    char c = 0;

    /* Check input from unified keyboard buffer first */
    if (input_has_char()) {
        c = input_dequeue();
    } else if (serial_received()) {
        /* Direct check on COM1 serial receiver for headless test input */
        c = serial_getc();
    }

    if (c == 0) return;

    if (c == '\r' || c == '\n') {
        vga_putc('\n');
        serial_putc('\n');
        line_buf[line_len] = '\0';
        shell_execute(line_buf);
        line_len = 0;
        line_buf[0] = '\0';
        print_prompt();
    } else if (c == '\b' || c == 127) {
        if (line_len > 0) {
            line_len--;
            line_buf[line_len] = '\0';
            vga_putc('\b');
            serial_puts("\b \b");
        }
    } else if (c >= 32 && c <= 126) {
        if (line_len + 1 < SHELL_MAX_CMD_LEN) {
            line_buf[line_len++] = c;
            line_buf[line_len] = '\0';
            vga_putc(c);
            serial_putc(c);
        }
    }
}
