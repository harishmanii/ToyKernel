#pragma once
#include <stdint.h>
#include "../include/stdlib.h"
#include "../include/stdio.h"
#include "../arch/i686/tss.h"
#include "../arch/i686/usermode.h"
#include "../mm/vmm/vmm.h"
#include "../mm/memory.h"
#include "../mm/pmm/pmm.h"

typedef struct Task {
    uint32_t pid;
    uint8_t  state;
    uint8_t  is_user;       /* 1 = ring-3 task, 0 = kernel task */
    /* 2 bytes padding – do NOT reorder; task.asm uses fixed offsets */
    struct Task *next;      /* offset 8  */
    uint32_t esp;           /* offset 12 – kernel stack ptr (used by switch_task) */
    uint32_t user_eip;      /* offset 16 – ring-3 entry point  (user tasks only) */
    uint32_t user_esp;      /* offset 20 – ring-3 stack top    (user tasks only) */
} Task;

typedef enum{
    TASK_READY      = 1,
    TASK_RUNNIG     = 2,
    TASK_PAUSED     = 3,
    TASK_COMPLETED  = 4,
    TASK_TERMINATED = 5,
    TASK_BLOCKED    = 6
} TASK_STATE;

#define STACK_SIZE 4096
extern Task *task ;

/* Virtual addresses for user-space mappings */
// it is our design
#define USER_CODE_VIRT   0x00400000   /* one page mapped for user code  */
#define USER_STACK_VIRT  0x00600000   /* one page mapped for user stack */

/* handler functions */
void init_task(void);
Task* create_task(void (*entry)());

/*
 * Create a ring-3 task.
 *   user_fn  – kernel-side function whose code will be mapped into user space.
 *              Must be self-contained (no kernel function calls from ring 3);
 *              use 'int $0x80' for syscalls.
 */
Task* create_user_task(void (*user_fn)());

void schedule();

void __attribute__((cdecl)) switch_task(Task* prev, Task *next);