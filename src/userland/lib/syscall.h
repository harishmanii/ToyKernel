/*
 * lib/syscall.h – Userland syscall interface
 *
 * Convention (matches kernel dispatcher in interrupts/syscall.c):
 *   EAX = syscall number
 *   EBX = first argument (if any)
 *   Return value → EAX
 *   Trigger: int $0x80
 *
 * DO NOT include any kernel headers here.
 */
#pragma once

#include <stdint.h>

/* -----------------------------------------------------------------------
 * Syscall numbers  (must stay in sync with kernel's syscalls[] table)
 * --------------------------------------------------------------------- */
#define SYS_SLEEP    2    /* sleep    EBX = milliseconds                  */
#define SYS_MALLOC   3    /* malloc   EBX = bytes  → EAX = ptr           */
#define SYS_FREE     4    /* free     EBX = ptr                           */
#define SYS_PUTC     5    /* putc     EBX = char value                    */
#define SYS_EXIT     6    /* exit current task, never returns             */
#define SYS_GETKEY   7    /* getkey   no args  → EAX = ASCII char        */


/* -----------------------------------------------------------------------
 * Raw syscall primitives (static inline – register setup + int $0x80)
 * --------------------------------------------------------------------- */

/* Syscall with no arguments */
static inline int32_t _syscall0(uint32_t num)
{
    int32_t ret;
    __asm__ __volatile__ (
        "int $0x80"
        : "=a" (ret)
        : "a"  (num)
        : "memory"
    );
    return ret;
}

/* Syscall with one argument passed in EBX */
static inline int32_t _syscall1(uint32_t num, uint32_t arg1)
{
    int32_t ret;
    __asm__ __volatile__ (
        "int $0x80"
        : "=a" (ret)
        : "a"  (num), "b" (arg1)
        : "memory"
    );
    return ret;
}


/* -----------------------------------------------------------------------
 * Syscall wrappers – implemented in lib/syscall.c
 * --------------------------------------------------------------------- */
void    sys_sleep(uint32_t ms);
void   *sys_malloc(uint32_t size);
void    sys_free(void *ptr);
void    sys_putc(char c);
__attribute__((noreturn)) void sys_exit(void);
char    sys_getkey(void);   /* blocks until a key is pressed, returns ASCII */
