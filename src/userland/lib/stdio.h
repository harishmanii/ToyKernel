/*
 * stdio.h – Userland I/O helpers
 *
 * Builds on sys_putc(). No kernel headers required.
 * Provides simple string helpers plus a small printf-compatible formatter.
 */
#pragma once

#include "syscall.h"
#include <stdarg.h>
#include <stdint.h>

#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_LENGTH_SHORT   2
#define PRINTF_STATE_LENGTH_LONG    3
#define PRINTF_STATE_SPEC           4

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4

static const char userland_hex_chars[] = "0123456789abcdef";

/* Write a single character */
static inline void putc(char c)
{
    sys_putc(c);
}

/* Write a NUL-terminated string */
static inline void print(const char *s)
{
    if (!s)
        s = "(null)";

    while (*s)
        putc(*s++);
}

/* Write a string followed by a newline */
static inline void println(const char *s)
{
    print(s);
    putc('\n');
}

static inline void printf_unsigned(unsigned long long number, int radix)
{
    char buffer[32];
    int pos = 0;

    do {
        unsigned long long rem = number % (unsigned long long)radix;
        number /= (unsigned long long)radix;
        buffer[pos++] = userland_hex_chars[rem];
    } while (number > 0);

    while (--pos >= 0)
        putc(buffer[pos]);
}

static inline void printf_signed(long long number, int radix)
{
    if (number < 0) {
        putc('-');
        printf_unsigned((unsigned long long)(-number), radix);
    } else {
        printf_unsigned((unsigned long long)number, radix);
    }
}

static inline void vprintf(const char *fmt, va_list args)
{
    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;
    int number = 0;
    int sign = 0;
    int radix = 10;

    if (!fmt)
        return;

    while (*fmt) {
        switch (state) {
            case PRINTF_STATE_NORMAL:
                switch (*fmt) {
                    case '%':
                        state = PRINTF_STATE_LENGTH;
                        break;
                    default:
                        putc(*fmt);
                        break;
                }
                break;

            case PRINTF_STATE_LENGTH:
                switch (*fmt) {
                    case 'h':
                        length = PRINTF_LENGTH_SHORT;
                        state = PRINTF_STATE_LENGTH_SHORT;
                        break;
                    case 'l':
                        length = PRINTF_LENGTH_LONG;
                        state = PRINTF_STATE_LENGTH_LONG;
                        break;
                    default:
                        goto PRINTF_STATE_SPEC_;
                }
                break;

            case PRINTF_STATE_LENGTH_SHORT:
                if (*fmt == 'h') {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                } else {
                    goto PRINTF_STATE_SPEC_;
                }
                break;

            case PRINTF_STATE_LENGTH_LONG:
                if (*fmt == 'l') {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                } else {
                    goto PRINTF_STATE_SPEC_;
                }
                break;

            case PRINTF_STATE_SPEC:
PRINTF_STATE_SPEC_:
                switch (*fmt) {
                    case 'c':
                        putc((char)va_arg(args, int));
                        break;

                    case 's':
                        print(va_arg(args, const char *));
                        break;

                    case '%':
                        putc('%');
                        break;

                    case 'd':
                    case 'i':
                        number = 1;
                        radix = 10;
                        sign = 1;
                        break;

                    case 'u':
                        number = 1;
                        radix = 10;
                        sign = 0;
                        break;

                    case 'X':
                    case 'x':
                    case 'p':
                        number = 1;
                        radix = 16;
                        sign = 0;
                        break;

                    case 'o':
                        number = 1;
                        radix = 8;
                        sign = 0;
                        break;

                    default:
                        break;
                }

                if (number) {
                    if (sign) {
                        switch (length) {
                            case PRINTF_LENGTH_SHORT_SHORT:
                            case PRINTF_LENGTH_SHORT:
                            case PRINTF_LENGTH_DEFAULT:
                                printf_signed((long long)va_arg(args, int), radix);
                                break;

                            case PRINTF_LENGTH_LONG:
                                printf_signed((long long)va_arg(args, long), radix);
                                break;

                            case PRINTF_LENGTH_LONG_LONG:
                                printf_signed(va_arg(args, long long), radix);
                                break;
                        }
                    } else {
                        switch (length) {
                            case PRINTF_LENGTH_SHORT_SHORT:
                            case PRINTF_LENGTH_SHORT:
                            case PRINTF_LENGTH_DEFAULT:
                                printf_unsigned((unsigned long long)va_arg(args, unsigned int), radix);
                                break;

                            case PRINTF_LENGTH_LONG:
                                printf_unsigned((unsigned long long)va_arg(args, unsigned long), radix);
                                break;

                            case PRINTF_LENGTH_LONG_LONG:
                                printf_unsigned(va_arg(args, unsigned long long), radix);
                                break;
                        }
                    }
                }

                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                radix = 10;
                sign = 0;
                number = 0;
                break;
        }

        fmt++;
    }
}

static inline void printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

/* Convenience wrappers kept for existing code */
static inline void print_uint(uint32_t n)
{
    printf("%u", n);
}

static inline void print_int(int32_t n)
{
    printf("%d", n);
}

static inline void print_hex(uint32_t n)
{
    print("0x");
    printf("%x", n);
}

/* -----------------------------------------------------------------------
 * readline – read a line of user input via keyboard interrupt syscall
 *
 * Blocks waiting for keystrokes delivered by the kernel keyboard driver
 * (SYS_GETKEY → syscall_getkey → get_key() in the kernel).
 * Each character is echoed immediately.  Enter ('\r'/'\n') ends the line.
 * Backspace (0x08) deletes the previous character (visual + buffer).
 * The result is stored in buf (max len-1 chars) and NUL-terminated.
 * Returns the number of characters stored (not counting the NUL).
 * --------------------------------------------------------------------- */
static inline uint32_t readline(char *buf, uint32_t len)
{
    uint32_t i = 0;

    while (1) {
        char c = sys_getkey();   /* blocks until key pressed (interrupt-driven) */

        if (c == '\r' || c == '\n') {
            /* Enter – end of line */
            sys_putc('\n');
            break;
        }

        if (c == '\b' || c == 127) {
            /* Backspace / DEL – erase last character */
            if (i > 0) {
                i--;
                /* Overwrite with space then move cursor back */
                sys_putc('\b');
                sys_putc(' ');
                sys_putc('\b');
            }
            continue;
        }

        if (c < 0x20) {
            /* Ignore other control characters */
            continue;
        }

        if (i < len - 1) {
            buf[i++] = c;
            sys_putc(c);   /* echo */
        }
        /* silently discard if buffer full */
    }

    buf[i] = '\0';
    return i;
}
