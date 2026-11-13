/**
 * CSE103-OS: PS/2 Keyboard Driver & Unified Input Queue
 * Translates Scancode Set 1 into ASCII characters, tracks shift state,
 * and maintains a circular input buffer shared between keyboard and serial COM1.
 */

#include "keyboard.h"
#include "irq.h"
#include "io.h"

#define KBD_DATA_PORT   0x60
#define KBD_STATUS_PORT 0x64

static char input_queue[INPUT_BUFFER_SIZE];
static volatile int queue_head = 0;
static volatile int queue_tail = 0;
static bool shift_pressed = false;

/* Scancode Set 1 standard lowercase/number map */
static const char kbd_scancode_map[128] = {
    0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   '-', 0,   0,   0,   '+', 0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0
};

/* Scancode Set 1 shifted uppercase/symbols map */
static const char kbd_scancode_shift_map[128] = {
    0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   '-', 0,   0,   0,   '+', 0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0
};

void input_enqueue(char c) {
    int next = (queue_head + 1) % INPUT_BUFFER_SIZE;
    if (next != queue_tail) {
        input_queue[queue_head] = c;
        queue_head = next;
    }
}

char input_dequeue(void) {
    if (queue_head == queue_tail) {
        return 0;
    }
    char c = input_queue[queue_tail];
    queue_tail = (queue_tail + 1) % INPUT_BUFFER_SIZE;
    return c;
}

bool input_has_char(void) {
    return queue_head != queue_tail;
}

static void keyboard_callback(registers_t *regs) {
    (void)regs;
    uint8_t scancode = inb(KBD_DATA_PORT);

    /* Left Shift (0x2A) or Right Shift (0x36) make/break */
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = true;
        return;
    }
    if (scancode == (0x2A | 0x80) || scancode == (0x36 | 0x80)) {
        shift_pressed = false;
        return;
    }

    /* Ignore key releases (break codes have bit 7 set) */
    if (scancode & 0x80) {
        return;
    }

    if (scancode < 128) {
        char ch = shift_pressed ? kbd_scancode_shift_map[scancode] : kbd_scancode_map[scancode];
        if (ch != 0) {
            input_enqueue(ch);
        }
    }
}

void keyboard_init(void) {
    queue_head = 0;
    queue_tail = 0;
    shift_pressed = false;

    irq_register_handler(1, keyboard_callback);
}
