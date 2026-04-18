#include "hal.h"

extern uint8_t __bss_start;
extern uint8_t __end;

void HAL_Initialize(BootParams *bootParams)
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));
    clrscr();
    i686_GDT_Initialize();
    tss_init();
    Initialize_memories(bootParams);
    remap_pic();
    init_idt_32();
    init_task();
    if (bootParams->ModuleCount > 0) {
        create_user_task_from_elf(bootParams->Modules[0].data);
    } else {
        printf("HAL: no userland module – check userland.elf on disk\n");
    }
    schedule();
}

void no_exit(void)
{
    for (;;)
        __asm__ volatile ("hlt");
}