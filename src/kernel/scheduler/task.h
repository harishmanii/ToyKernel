#pragma once
#include <stdint.h>
#include "../include/stdlib.h"
#include "../include/stdio.h"

typedef struct Task {
    uint32_t pid;
    uint8_t state;
    struct Task *next;
     uint32_t esp;
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

/* handler functions */
void init_task(void);
Task* create_task(void (*entry)());
void schedule();

void __attribute__((cdecl)) switch_task(Task* prev,Task *next);