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

    // loads userland
    g_BootParams.ModuleCount = 0;
    uint8_t *userlandBuf = (uint8_t *)MEMORY_USERLAND_ADDR;
    FAT_File *ufd = FAT_Open(&disk, "/userland.elf");
    if (ufd)
    {
        uint32_t totalRead = 0;
        while ((readSize = FAT_Read(&disk, ufd, MEMORY_LOAD_SIZE, KernelLoadBuffer)))
        {
            memcpy(userlandBuf + totalRead, KernelLoadBuffer, readSize);
            totalRead += readSize;
        }
        FAT_Close(ufd);

        g_BootParams.Modules[0].data = (void *)MEMORY_USERLAND_ADDR;
        g_BootParams.Modules[0].size = totalRead;
        const char *modname = "userland.elf";
        uint32_t ni = 0;
        while (modname[ni] && ni < 31) {
            g_BootParams.Modules[0].name[ni] = modname[ni];
            ni++;
        }
        g_BootParams.Modules[0].name[ni] = '\0';
        g_BootParams.ModuleCount = 1;
        printf("Loaded userland.elf (%u bytes at 0x%x)\r\n",
               totalRead, (uint32_t)MEMORY_USERLAND_ADDR);
    }
    else
    {
        printf("Warning: userland.elf not found on disk\r\n");
    }

    i686_GDT_Initialize();
    Initialize_memories(&g_BootParams);

    for (uint32_t virt = KERNEL_ADDRESS; virt < 0x400000; virt += PAGE_SIZE)
        unmap_page((void *)virt);
    __asm__ __volatile__ ("movl %CR3, %ECX; movl %ECX, %CR3");

    KernelStart kernelStart = (KernelStart)0xC0000000;
    kernelStart(&g_BootParams);

end:
    for (;;);
}