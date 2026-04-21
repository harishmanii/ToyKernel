#include "syscall.h"
#include "../scheduler/task.h"
#include "../keyboard/keyboard.h"


#define MAX_SYSCALLS 8

// Test syscall 0
void syscall_test0(syscall_saved_regs_t *regs)
{
    (void)regs;
    printf("SYSCALL testing 0");
}

void syscall_test1(syscall_saved_regs_t *regs)
{
    (void)regs;
    printf("SYSCALL testing 1");
}

// Print a single character passed in EBX.
void syscall_print(syscall_saved_regs_t *regs)
{
    put_c((char)regs->ebx);
}

// Sleep for a given number of milliseconds.
// INPUT:  EBX = number of milliseconds
void syscall_sleep(syscall_saved_regs_t *regs)
{
    //sleep_second(regs->ebx);
}



// Memory

// Allocate uninitialized memory.
// INPUT:   EBX = size in bytes to allocate
// OUTPUT:  EAX = pointer to allocated memory (0 on failure)
void syscall_malloc(syscall_saved_regs_t *regs)
{
    uint32_t bytes = regs->ebx;

    // First malloc() from the calling program?
    if (!malloc_list_head)
        malloc_init(bytes); // Yes, set up initial memory/linked list

    void *ptr = malloc_next_block(bytes);

    merge_free_blocks();    // Combine consecutive free blocks of memory

    // Return pointer to malloc-ed memory via EAX
    regs->eax = (uint32_t)ptr;
}

// Free allocated memory at a pointer.
// INPUT:  EBX = pointer to malloc-ed bytes
void syscall_free(syscall_saved_regs_t *regs)
{
    malloc_free((void *)regs->ebx);
}


// Exit the current user task cleanly.
// Marks the task COMPLETED and yields to the scheduler.
void syscall_exit(syscall_saved_regs_t *regs)
{
    (void)regs;
    current_task->state = TASK_COMPLETED;
    schedule();
    /* schedule() never returns here */
}

// Return the next ASCII keypress to userland (blocking).
// Delegates to the kernel keyboard driver which waits via HLT.
// OUTPUT: EAX = ASCII key code
void syscall_getkey(syscall_saved_regs_t *regs)
{
    regs->eax = (uint32_t)get_key();   // get_key() blocks until a key is pressed
}

void *syscalls[MAX_SYSCALLS] = {
    syscall_test0,
    syscall_test1,
    syscall_sleep,
    syscall_malloc,
    syscall_free,
    syscall_print,
    syscall_exit,       /* SYS_EXIT   = 6 */
    syscall_getkey      /* SYS_GETKEY = 7 */
};

__attribute__ ((naked)) void  syscall_dispatcher(int_frame_32_t *frame)
{
    // "basic" syscall handler, push everything we want to save, call the syscall by
    //   offsetting into syscalls table with value in eax, then pop everything back 
    //   and return using "iret" (d/q), NOT regualar "ret" as this is technically
    //   an interrupt (software interrupt)
    //
    // Already on stack: SS, SP, FLAGS, CS, IP
    // Need to push: AX, GS, FS, ES, DS, BP, DI, SI, DX, CX, BX
    
   __asm__ __volatile__ (".intel_syntax noprefix\n"

                          ".equ MAX_SYSCALLS, 8\n"  // Have to define again, inline asm does not see the #define

                          "cmp eax, MAX_SYSCALLS-1\n"   // syscalls table is 0-based
                          "ja invalid_syscall\n"        // invalid syscall number, skip and return

                          "push eax\n"
                          "push gs\n"
                          "push fs\n"
                          "push es\n"
                          "push ds\n"
                          "push ebp\n"
                          "push edi\n"
                          "push esi\n"
                          "push edx\n"
                          "push ecx\n"
                          "push ebx\n"
                          "push esp\n"         // arg0: pointer to saved regs (syscall_saved_regs_t*)
                          "call [syscalls+eax*4]\n"
                          "add esp, 4\n"       // discard the pushed esp argument
                          "pop ebx\n"
                          "pop ecx\n"
                          "pop edx\n"
                          "pop esi\n"
                          "pop edi\n"
                          "pop ebp\n"
                          "pop ds\n"
                          "pop es\n"
                          "pop fs\n"
                          "pop gs\n"
                          "pop eax\n"          // restore EAX (may carry return value written by syscall)
                          "iretd\n"            // interrupt return, NOT regular ret

                          "invalid_syscall:\n"
                          "mov eax, -1\n"   // Error will be -1
                          "iretd\n"

                          ".att_syntax");
}