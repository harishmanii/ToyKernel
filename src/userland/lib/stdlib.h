/*
 * stdlib.h – Userland memory helpers
 *
 * Thin wrappers around SYS_MALLOC / SYS_FREE.
 * No kernel headers required.
 */
#pragma once

#include "syscall.h"
#include <stdint.h>

/*
 * Allocate size bytes of memory.
 * Returns a pointer on success, NULL on failure.
 */
static inline void *malloc(uint32_t size)
{
    return sys_malloc(size);
}

/*
 * Free memory previously returned by malloc().
 */
static inline void free(void *ptr)
{
    sys_free(ptr);
}

/*
 * Allocate a zero-initialised block (nmemb * size bytes).
 */
static inline void *calloc(uint32_t nmemb, uint32_t size)
{
    uint32_t  total = nmemb * size;
    uint8_t  *ptr   = (uint8_t *)sys_malloc(total);
    if (!ptr) return (void *)0;

    for (uint32_t i = 0; i < total; i++)
        ptr[i] = 0;

    return (void *)ptr;
}
