/*
 * lib/syscall.c – Userland syscall wrappers
 *
 * Fires int $0x80 with the appropriate syscall number and arguments.
 * The kernel handles the request and irets back to ring 3.
 * No kernel headers included.
 */

#include "syscall.h"


void sys_sleep(uint32_t ms)
{
    _syscall1(SYS_SLEEP, ms);
}

void *sys_malloc(uint32_t size)
{
    return (void *)(uintptr_t)_syscall1(SYS_MALLOC, size);
}

void sys_free(void *ptr)
{
    _syscall1(SYS_FREE, (uint32_t)(uintptr_t)ptr);
}

void sys_putc(char c)
{
    _syscall1(SYS_PUTC, (uint32_t)(unsigned char)c);
}

/*
 * sys_getkey – block until the kernel keyboard driver delivers a keystroke.
 * Returns the ASCII character of the key pressed.
 * Uses SYS_GETKEY (7) which calls syscall_getkey() → get_key() in the kernel.
 */
char sys_getkey(void)
{
    return (char)_syscall0(SYS_GETKEY);
}

__attribute__((noreturn)) void sys_exit(void)
{
    _syscall0(SYS_EXIT);
    __builtin_unreachable();
}
