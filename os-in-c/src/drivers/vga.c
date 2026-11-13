/**
 * CSE103-OS: VGA 80x25 Color Text Mode Driver
 * Manages frame buffer at physical address 0xB8000, hardware text cursor,
 * color attribute bytes, backspace editing, and automatic line scrolling.
 */

#include "vga.h"
#include "io.h"
#include "string.h"

static size_t vga_row = 0;
static size_t vga_column = 0;
static uint8_t vga_color = 0x0F; /* Default: White on Black */
static volatile uint16_t *const vga_buffer = VGA_MEMORY;

void vga_update_cursor(int row, int col) {
    uint16_t pos = (uint16_t)(row * VGA_WIDTH + col);

    /*
     * Hardware cursor update on VGA CRT Controller registers.
     * Register 0x0E is cursor location high byte; register 0x0F is low byte.
     */
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_set_color(vga_color_t fg, vga_color_t bg) {
    vga_color = vga_entry_color(fg, bg);
}

void vga_clear(void) {
    uint16_t blank = vga_entry(' ', vga_color);
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = blank;
        }
    }
    vga_row = 0;
    vga_column = 0;
    vga_update_cursor((int)vga_row, (int)vga_column);
}

static void vga_scroll(void) {
    uint16_t blank = vga_entry(' ', vga_color);
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[(y - 1) * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;
    }
    vga_row = VGA_HEIGHT - 1;
}

void vga_putc(char c) {
    if (c == '\n') {
        vga_column = 0;
        if (++vga_row >= VGA_HEIGHT) {
            vga_scroll();
        }
    } else if (c == '\r') {
        vga_column = 0;
    } else if (c == '\t') {
        size_t tab_stop = (vga_column + 8) & ~7;
        while (vga_column < tab_stop && vga_column < VGA_WIDTH) {
            vga_buffer[vga_row * VGA_WIDTH + vga_column] = vga_entry(' ', vga_color);
            vga_column++;
        }
        if (vga_column >= VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row >= VGA_HEIGHT) {
                vga_scroll();
            }
        }
    } else if (c == '\b') {
        if (vga_column > 0) {
            vga_column--;
            vga_buffer[vga_row * VGA_WIDTH + vga_column] = vga_entry(' ', vga_color);
        } else if (vga_row > 0) {
            vga_row--;
            vga_column = VGA_WIDTH - 1;
            vga_buffer[vga_row * VGA_WIDTH + vga_column] = vga_entry(' ', vga_color);
        }
    } else {
        vga_buffer[vga_row * VGA_WIDTH + vga_column] = vga_entry((unsigned char)c, vga_color);
        if (++vga_column >= VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row >= VGA_HEIGHT) {
                vga_scroll();
            }
        }
    }
    vga_update_cursor((int)vga_row, (int)vga_column);
}

void vga_puts(const char *str) {
    if (!str) return;
    while (*str) {
        vga_putc(*str++);
    }
}

void vga_init(void) {
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_clear();
}
