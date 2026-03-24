#include "task.h"

Task *task ;

Task *current_task;

static uint16_t next_taskId = 0;

//0xc0000060
extern void task_entry_trampoline(void);

void idle_task()
{
   
    printf("Task is on IDLE state");
    for (;;)
        __asm__("hlt");
}

void task_exit(){
    printf("exit\n");
    current_task->state = TASK_COMPLETED;
    schedule();

    for(;;);
}


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
    task->pid      = next_taskId++;
    task->state    = TASK_READY;
    task->is_user  = 0;
    task->next     = 0;
    task->user_eip = 0;
    task->user_esp = 0;
    task->esp      = (uint32_t) setup_stack(mem,idle_task);

    current_task = task;
}

Task* create_task(void (*entry)()){
    uint8_t *mem = malloc(sizeof(Task)+STACK_SIZE); // used uint_8 for exact byte calculation
     //TODO: null check
    Task *t = (Task*)mem;
    t->esp = (uint32_t) setup_stack(mem,entry);
    t->state = TASK_READY;
    t->is_user = 0;
    t->next = 0;
    t->user_eip = 0;
    t->user_esp = 0;

    
    Task *curr_task = task;

    while(curr_task->next !=0){
         curr_task = curr_task->next;
    }
    t->pid = next_taskId++;
    curr_task->next = t;
    
    return t;
}

/* ------------------------------------------------------------------
 * Ring-3 task support
 * ------------------------------------------------------------------
 *
 * When a user task gets scheduled, switch_task() restores its kernel
 * stack and returns into task_entry_trampoline, which enables
 * interrupts and then rets into usermode_trampoline().
 * usermode_trampoline() points TSS.esp0 at the top of the kernel
 * stack (so the CPU can find it on the next ring-0 entry) and then
 * calls enter_usermode() which irets to ring 3 – it never returns.
 */
static void usermode_trampoline(void)
{
    enter_usermode(current_task->user_eip, current_task->user_esp);
    /* never reached */
}

Task* create_user_task(void (*user_fn)())
{
     /* --- map user code page --------------------------------------- */
   // currently the fn_addr in kernel addr so ring3 user can't have the access so we need to remap this with ring3 attribute
    uint32_t fn_virt      = (uint32_t)user_fn;
    // before map we need to get the phys addr of the function , remember our page size is 4kb which is 4096bytes which means 12bits which is 0xFFFF so we can only addr 12 bits of index only
    uint32_t fn_page_phys = VIRT_TO_PHYS(fn_virt);  /* page-align */
    fn_page_phys = fn_page_phys & ~0xFFFu;
    // extract the offset so we can use the offset to get the exact addr
    uint32_t fn_offset    = fn_virt & 0xFFFu;                  /* byte offset within page */

    // USER_CODE_VIRT = 0x40000 is our own design because every process has own virtual memory so it will not be problem
    map_page_user((void *)fn_page_phys, (void *)USER_CODE_VIRT);
    // user_eip now points to user_fn page table
    uint32_t user_eip = USER_CODE_VIRT + fn_offset;

    /* --- map user stack page --------------------------------------- */
    uint32_t stack_phys = (uint32_t)allocate_block(1);
    map_page_user((void *)stack_phys, (void *)USER_STACK_VIRT);
    uint32_t user_esp = USER_STACK_VIRT + PAGE_SIZE; /* stack top (grows down) */

    /* --- create kernel task whose entry calls usermode_trampoline - */
    uint8_t *mem = malloc(sizeof(Task) + STACK_SIZE);
    Task *t      = (Task *)mem;
    t->state     = TASK_READY;
    t->is_user   = 1;
    t->next      = 0;
    t->user_eip  = user_eip;
    t->user_esp  = user_esp;
    t->esp       = (uint32_t)setup_stack(mem, usermode_trampoline);
    t->pid       = next_taskId++;

    Task *curr = task;
    while (curr->next) curr = curr->next;
    curr->next = t;

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

    // Keep TSS.esp0 pointing to the top of the new task's kernel stack.
    // The CPU reads this on every Ring 3 → Ring 0 transition (interrupt /
    // syscall) so it knows where to put the exception frame.
    tss_set_kernel_stack((uint32_t)((uint8_t*)current_task + sizeof(Task) + STACK_SIZE));

    switch_task(prev,current_task);
}