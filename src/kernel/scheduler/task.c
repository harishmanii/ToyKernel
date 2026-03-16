#include "task.h"

Task *task ;

Task *current_task;

static uint16_t next_taskId = 0;

//0xc0000060
extern void task_entry_trampoline(void);

void idle_task()
{
    //0xc0000c36
    printf("Task is on IDLE state");
    for (;;)
        __asm__("hlt");
}

void task_exit(){
    //0xc0000c3c
    printf("exit\n");
    current_task->state = TASK_COMPLETED;
    schedule();

    for(;;);
}

//task1 0xc0000b93
//task2 0xc0000bab

uint32_t* setup_stack(uint8_t* mem,void (*entry)()){
    uint32_t *stack = (uint32_t *)(mem+sizeof(Task)+STACK_SIZE);
    *(--stack) = (uint32_t) task_exit;
    *(--stack) = (uint32_t) entry;
    *(--stack) = (uint32_t) task_entry_trampoline;

    *(--stack) = 0; // EBP
    *(--stack) = 0; // EBX
    *(--stack) = 0; // ESI
    *(--stack) = 0; // EDI
    return stack;
}


void init_task(void){
    uint8_t *mem = malloc(sizeof(Task)+STACK_SIZE);
    //TODO: null check
    task = (Task*)mem;

    //initial task 
    task->pid = next_taskId++;
    task->state = TASK_READY;
    task->next = 0;
    task->esp = (uint32_t) setup_stack(mem,idle_task);

    current_task = task;
}

Task* create_task(void (*entry)()){
    uint8_t *mem = malloc(sizeof(Task)+STACK_SIZE); // used uint_8 for exact byte calculation
     //TODO: null check
    Task *t = (Task*)mem;
    t->esp = (uint32_t) setup_stack(mem,entry);
    t->state = TASK_READY;
    t->next = 0;

    
    Task *curr_task = task;

    while(curr_task->next !=0){
         curr_task = curr_task->next;
    }
    t->pid = next_taskId++;
    curr_task->next = t;
    
    return t;
}

void schedule(){
    Task *prev = current_task;
    if(prev->state != TASK_COMPLETED){
        prev->state = TASK_PAUSED;
    }
    do {
        current_task = current_task->next;

        if(!current_task)
            current_task = task;

    } while(current_task->state == TASK_COMPLETED);

    
    current_task->state = TASK_RUNNIG;

    switch_task(prev,current_task);
}