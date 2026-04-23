#include "task.h"

Task *task ;

Task *current_task;

static uint16_t next_taskId = 0;

extern void task_entry_trampoline(void);

void idle_task(void)
{
    for (;;)
        __asm__ volatile ("sti\n\thlt");  /* sleep until the next interrupt, then loop */
}

void task_exit(void)
{
    current_task->state = TASK_COMPLETED;
    for (;;);
}


uint32_t* setup_stack(uint8_t* mem,void (*entry)()){
    uint32_t *stack = (uint32_t *)(mem+sizeof(Task)+STACK_SIZE);
    *(--stack) = (uint32_t) task_exit;
    *(--stack) = (uint32_t) entry;
    *(--stack) = (uint32_t) task_entry_trampoline;

    /* Must match switch_task's push order: pushfd, push ebp, ebx, esi, edi
     * Pop order (reverse): pop edi, pop esi, pop ebx, pop ebp, popfd, ret
     * Stack grows DOWN, so the FIRST pushed (EFLAGS) is at the HIGHEST address: */
    *(--stack) = 0x200; // EFLAGS: IF=1 (interrupts enabled) — restored by popfd
    *(--stack) = 0; // EBP
    *(--stack) = 0; // EBX
    *(--stack) = 0; // ESI
    *(--stack) = 0; // EDI   ← ESP points here (top of stack)
    return stack;
}


void init_task(void)
{
    uint8_t *mem = malloc(sizeof(Task) + STACK_SIZE);
    task = (Task *)mem;
    task->pid      = next_taskId++;
    task->state    = TASK_READY;
    task->is_user  = 0;
    task->next     = 0;
    task->user_eip = 0;
    task->user_esp = 0;
    task->esp      = (uint32_t)setup_stack(mem, idle_task);
    task->priority = LOW_TASK;
    task->time_slice = task->priority;
    task->wake_tick  = 0;
    current_task = task;
}

Task *create_task(void (*entry)(void))
{
    uint8_t *mem = malloc(sizeof(Task) + STACK_SIZE);
    Task *t = (Task *)mem;
    t->esp     = (uint32_t)setup_stack(mem, entry);
    t->state   = TASK_READY;
    t->is_user = 0;
    t->next    = 0;
    t->user_eip = 0;
    t->user_esp = 0;
    t->pid     = next_taskId++;
    t->priority     = CRITICAL_TASK;
    t->time_slice = t->priority;
    t->wake_tick  = 0;

    Task *curr = task;
    while (curr->next) curr = curr->next;
    curr->next = t;
    return t;
}

static void usermode_trampoline(void)
{
    enter_usermode(current_task->user_eip, current_task->user_esp);
}

Task *create_user_task_from_elf(const void *elf_data)
{
    uint32_t entry = 0;
    int ret = elf_load(elf_data, &entry);
    if (ret != ELF_OK) {
        printf("create_user_task_from_elf: ELF load failed (err=%d)\n", ret);
        return 0;
    }
    uintptr_t stack_phys = allocate_block(1);
    map_page_user((void *)stack_phys, (void *)USER_STACK_VIRT);
    uint32_t user_esp = USER_STACK_VIRT + PAGE_SIZE;

    uint8_t *mem = malloc(sizeof(Task) + STACK_SIZE);
    Task    *t   = (Task *)mem;
    t->state     = TASK_READY;
    t->is_user   = 1;
    t->next      = 0;
    t->user_eip  = entry;         
    t->user_esp  = user_esp;
    t->esp       = (uint32_t)setup_stack(mem, usermode_trampoline);
    t->pid       = next_taskId++;
    t->priority = MEDIUM_TASK;
    t->time_slice = t->priority;
    t->wake_tick  = 0;

    /* Append to task list */
    Task *curr = task;
    while (curr->next) curr = curr->next;
    curr->next = t;

    return t;
}

void schedule(void)
{
    Task *prev = current_task;
    if (prev->state != TASK_COMPLETED && prev->state != TASK_BLOCKED)
        prev->state = TASK_PAUSED;
    /* Walk the list until we find a runnable task.
     * If every task is BLOCKED or COMPLETED, halt with interrupts enabled
     * so the timer ISR can unblock a sleeper and we re-enter schedule(). */
    Task *start = current_task;
    do {
        current_task = current_task->next;
        if (!current_task)
            current_task = task;
        if (current_task == start) {
            /* Full loop with no runnable task – spin on HLT until timer unblocks one */
            __asm__ volatile ("sti");
            while (current_task->state == TASK_COMPLETED ||
                   current_task->state == TASK_BLOCKED)
                __asm__ volatile ("hlt");
            __asm__ volatile ("cli");
        }
    } while (current_task->state == TASK_COMPLETED ||
             current_task->state == TASK_BLOCKED);
    current_task->state = TASK_RUNNIG;
    tss_set_kernel_stack((uint32_t)((uint8_t *)current_task + sizeof(Task) + STACK_SIZE));
    switch_task(prev, current_task);
}