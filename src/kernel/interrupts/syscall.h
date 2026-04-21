#pragma once

#include "../include/stdio.h"
#include "../mm/handler/malloc.h"
#include "exceptions.h"
#include "pic.h"
#include "../drivers/display/screen.h"

// Saved general-purpose registers pushed by syscall_dispatcher.
// Layout mirrors the push order (last pushed = first field = lowest address).
typedef struct {
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t ds;
    uint32_t es;
    uint32_t fs;
    uint32_t gs;
    uint32_t eax;   // syscall number on entry; return value on exit
} __attribute__((packed)) syscall_saved_regs_t;

void syscall_test0(syscall_saved_regs_t *regs);
void syscall_test1(syscall_saved_regs_t *regs);
void syscall_sleep(syscall_saved_regs_t *regs);
void syscall_print(syscall_saved_regs_t *regs);
void syscall_exit(syscall_saved_regs_t *regs);
void syscall_getkey(syscall_saved_regs_t *regs);
void syscall_malloc(syscall_saved_regs_t *regs);
void syscall_free(syscall_saved_regs_t *regs);
__attribute__ ((naked)) void syscall_dispatcher(int_frame_32_t *frame);