/**
 * CSE103-OS: Formatted Kernel Printf Routine
 * Writes formatted output to both VGA console and COM1 serial simultaneously.
 */

#include "kprintf.h"
#include "string.h"
#include "vga.h"
#include "serial.h"

typedef __builtin_va_list va_list;
#define va_start(v, l) __builtin_va_start(v, l)
#define va_arg(v, l)   __builtin_va_arg(v, l)
#define va_end(v)      __builtin_va_end(v)

static void print_char_dual(char c) {
    vga_putc(c);
    serial_putc(c);
}

static void print_string_dual(const char *s) {
    if (!s) s = "(null)";
    while (*s) {
        print_char_dual(*s++);
    }
}

static void print_num_dual(uint32_t num, int base, bool uppercase, bool is_signed) {
    char buf[32];
    int i = 0;
    const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    if (is_signed && (int32_t)num < 0) {
        print_char_dual('-');
        num = (uint32_t)(-(int32_t)num);
    }

    if (num == 0) {
        print_char_dual('0');
        return;
    }

    while (num > 0 && i < 31) {
        buf[i++] = digits[num % base];
        num /= base;
    }

    while (i > 0) {
        print_char_dual(buf[--i]);
    }
}

int kprintf(const char *fmt, ...) {
    if (!fmt) return 0;
    va_list args;
    va_start(args, fmt);

    int count = 0;
    for (size_t i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] != '%') {
            print_char_dual(fmt[i]);
            count++;
            continue;
        }

        i++; /* Skip '%' */
        switch (fmt[i]) {
            case 'c': {
                char c = (char)va_arg(args, int);
                print_char_dual(c);
                count++;
                break;
            }
            case 's': {
                const char *s = va_arg(args, const char *);
                print_string_dual(s);
                count += (s ? (int)strlen(s) : 6);
                break;
            }
            case 'd':
            case 'i': {
                int32_t val = va_arg(args, int32_t);
                print_num_dual((uint32_t)val, 10, false, true);
                count++;
                break;
            }
            case 'u': {
                uint32_t val = va_arg(args, uint32_t);
                print_num_dual(val, 10, false, false);
                count++;
                break;
            }
            case 'x': {
                uint32_t val = va_arg(args, uint32_t);
                print_num_dual(val, 16, false, false);
                count++;
                break;
            }
            case 'X': {
                uint32_t val = va_arg(args, uint32_t);
                print_num_dual(val, 16, true, false);
                count++;
                break;
            }
            case 'p': {
                uint32_t val = va_arg(args, uint32_t);
                print_string_dual("0x");
                print_num_dual(val, 16, false, false);
                count += 2;
                break;
            }
            case '%': {
                print_char_dual('%');
                count++;
                break;
            }
            default: {
                print_char_dual('%');
                print_char_dual(fmt[i]);
                count += 2;
                break;
            }
        }
    }

    va_end(args);
    return count;
}

int ksnprintf(char *buf, size_t size, const char *fmt, ...) {
    if (!buf || size == 0 || !fmt) return 0;
    va_list args;
    va_start(args, fmt);

    size_t out_idx = 0;
    for (size_t i = 0; fmt[i] != '\0' && out_idx + 1 < size; i++) {
        if (fmt[i] != '%') {
            buf[out_idx++] = fmt[i];
            continue;
        }

        i++;
        if (fmt[i] == 's') {
            const char *s = va_arg(args, const char *);
            if (!s) s = "(null)";
            while (*s && out_idx + 1 < size) {
                buf[out_idx++] = *s++;
            }
        } else if (fmt[i] == 'c') {
            char c = (char)va_arg(args, int);
            buf[out_idx++] = c;
        } else if (fmt[i] == 'd' || fmt[i] == 'i') {
            int32_t val = va_arg(args, int32_t);
            char tmp[16];
            int ti = 0;
            if (val < 0) {
                if (out_idx + 1 < size) buf[out_idx++] = '-';
                val = -val;
            }
            if (val == 0) {
                if (out_idx + 1 < size) buf[out_idx++] = '0';
            } else {
                while (val > 0 && ti < 15) {
                    tmp[ti++] = (char)('0' + (val % 10));
                    val /= 10;
                }
                while (ti > 0 && out_idx + 1 < size) {
                    buf[out_idx++] = tmp[--ti];
                }
            }
        } else if (fmt[i] == '%') {
            buf[out_idx++] = '%';
        }
    }

    buf[out_idx] = '\0';
    va_end(args);
    return (int)out_idx;
}
