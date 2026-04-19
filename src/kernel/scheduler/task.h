#pragma once
#include <stdint.h>
#include "../include/stdlib.h"
#include "../include/stdio.h"
#include "../arch/i686/tss.h"
#include "../arch/i686/usermode.h"
#include "../mm/vmm/vmm.h"
#include "../mm/memory.h"
#include "../mm/pmm/pmm.h"
#include "../include/string.h"
#include "../loader/elf.h"

typedef struct Task {
    uint32_t pid;
    uint8_t  state;
    uint8_t  is_user;       /* 1 = ring-3 task, 0 = kernel task */
    /* 2 bytes padding – do NOT reorder; task.asm uses fixed offsets */
    struct Task *next;      /* offset 8  */
    uint32_t esp;           /* offset 12 – kernel stack ptr (used by switch_task) */
    uint32_t user_eip;      /* offset 16 – ring-3 entry point  (user tasks only) */
    uint32_t user_esp;      /* offset 20 – ring-3 stack top    (user tasks only) */
    uint8_t priority;
    uint16_t time_slice;
} Task;

typedef enum{
    TASK_READY      = 1,
    TASK_RUNNIG     = 2,
    TASK_PAUSED     = 3,
    TASK_COMPLETED  = 4,
    TASK_TERMINATED = 5,
    TASK_BLOCKED    = 6
} TASK_STATE;

typedef enum{
    LOW_TASK = 5,
    MEDIUM_TASK = 15,
    HIGH_TASK = 30,
    CRITICAL_TASK = 60
} TASK_PRIORITY;

#define STACK_SIZE 4096
extern Task *task;

#define USER_CODE_VIRT   0x00400000   /* virtual base for user code  */
#define USER_STACK_VIRT  0x00600000   /* virtual base for user stack */

extern Task *current_task;

/* handler functions */
void init_task(void);
Task* create_task(void (*entry)());


Task* create_user_task_from_elf(const void *elf_data);

Task* create_user_task(void (*user_fn)());

void schedule();

void __attribute__((cdecl)) switch_task(Task* prev, Task *next);