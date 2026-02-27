#include <stdint.h>
#include "stdio.h"
#include "x86.h"
#include "disk.h"
#include "fat.h"
#include "memdefs.h"
#include "string.h"
#include "memdetects.h"
#include "bootparams.h"

#include "memory.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "mm/vmm/vmm.h"


uint8_t* KernelLoadBuffer = (uint8_t*)MEMORY_LOAD_KERNEL;
uint8_t* Kernel = (uint8_t*)MEMORY_KERNEL_ADDR;

BootParams g_BootParams;

typedef void (*KernelStart)(BootParams* bootParams);

void __attribute__((cdecl)) start(uint16_t bootDrive)
{
    clrscr();

    DISK disk;
    if (!DISK_Initialize(&disk, bootDrive))
    {
        printf("Disk init error\r\n");
        goto end;
    }

    if (!FAT_Initialize(&disk))
    {
        printf("FAT init error\r\n");
        goto end;
    }

    g_BootParams.BootDevice = bootDrive;

    // load kernel
    FAT_File* fd = FAT_Open(&disk, "/kernel.bin");
    Memory_Detect(&g_BootParams.Memory);

    uint32_t readSize;
    uint8_t* kernelBuffer = Kernel;
    while ((readSize = FAT_Read(&disk, fd, MEMORY_LOAD_SIZE, KernelLoadBuffer)))
    {
        memcpy(kernelBuffer, KernelLoadBuffer, readSize);
        kernelBuffer += readSize;
    }
    FAT_Close(fd);

    i686_GDT_Initialize(); // initialize and add the entry in GDT
    // i686_IDT_Initialize(); // init the IDT table
    // i686_ISR_Initialize(); // init the entry with interrupts
    Initialize_memories(&g_BootParams);

    for (uint32_t virt = KERNEL_ADDRESS; virt < 0x400000; virt += PAGE_SIZE)
        unmap_page((void *)virt);

    
    __asm__ __volatile__ ("movl %CR3, %ECX; movl %ECX, %CR3");
    // *(uint32_t *)CURRENT_PAGE_DIR_ADDRESS = (uint32_t)current_page_directory;


    // printf("The value is %p ",Kernel);
    // print_buffer("entry bytes:", Kernel, 8);
    // // execute kernel
    KernelStart kernelStart = (KernelStart)0xC0000000;
    kernelStart(&g_BootParams);

end:
    for (;;);
}