/*

$ ../../opt/cross/i686-elf/bin/i686-elf-readelf -h userland.elf 
ELF Header:
  Magic:   7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF32
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           Intel 80386
  Version:                           0x1
  Entry point address:               0x40011d
  Start of program headers:          52 (bytes into file)
  Start of section headers:          8284 (bytes into file)
  Flags:                             0x0
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         1
  Size of section headers:           40 (bytes)
  Number of section headers:         14
  Section header string table index: 13

*/

#include "elf.h"
#include "../mm/vmm/vmm.h"
#include "../mm/pmm/pmm.h"
#include "../include/string.h"
#include "../include/stdio.h"

int elf_load(const void *data, uint32_t *entry_out)
{
    const Elf32_Ehdr *ehdr = (const Elf32_Ehdr *)data;

   // ELF file valitaion
    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr->e_ident[EI_MAG3] != ELFMAG3) {
        printf("elf_load: bad magic\n");
        return ELF_ERR_MAGIC;
    }

    // arch check whether it is 32bit
    if (ehdr->e_ident[EI_CLASS] != ELFCLASS32) {
        printf("elf_load: not ELF32\n");
        return ELF_ERR_CLASS;
    }

    // check what kind of file is this 
    /*
        ET_EXEC → executable
        ET_DYN → shared lib (.so)
        ET_REL → object file (.o)
    */
    if (ehdr->e_type != ET_EXEC) {
        printf("elf_load: not an executable ELF (type=%u)\n",
               (unsigned)ehdr->e_type);
        return ELF_ERR_TYPE;
    }

    //TODO: need to add check if it is for arm or x86 32 or x86 64

    const uint8_t *base  = (const uint8_t *)data;
    int loaded = 0;

    for (int i = 0; i < (int)ehdr->e_phnum; i++) {

        /*
        
            Entry point 0x40011d
            There is 1 program header, starting at offset 52

            Program Headers:
            Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
            LOAD           0x001000 0x00400000 0x00400000 0x002f5 0x002f5 R E 0x1000
        
        */

        const Elf32_Phdr *phdr = (const Elf32_Phdr *)(base + ehdr->e_phoff + (uint32_t)i * ehdr->e_phentsize);

        if (phdr->p_type != PT_LOAD)
            continue;   /* skip PT_NOTE, PT_GNU_STACK, etc. */

        uint32_t vaddr  = phdr->p_vaddr;   /* virtual base of this segment */
        uint32_t filesz = phdr->p_filesz;  /* bytes from the file          */
        uint32_t memsz  = phdr->p_memsz;   /* bytes in memory (>= filesz)  */

        uint32_t num_pages = (memsz + PAGE_SIZE - 1) / PAGE_SIZE;

        for (uint32_t p = 0; p < num_pages; p++) {
            // allocating a fresh page for this program
            uintptr_t phys = allocate_block(1);
            if (!phys) {
                printf("elf_load: out of physical memory\n");
                return ELF_ERR_OOM;
            }

            uint32_t page_off = p * PAGE_SIZE;   /* offset into segment */

    
            if (page_off < filesz) {
                uint32_t copy = filesz - page_off;
                if (copy > PAGE_SIZE)
                    copy = PAGE_SIZE;

                memcpy((void *)phys,base + phdr->p_offset + page_off,copy);
    
            }

            map_page_user((void *)phys,(void *)(vaddr + page_off));
        }

        loaded++;
    }

    if (loaded == 0) {
        printf("elf_load: no PT_LOAD segments found\n");
        return ELF_ERR_NO_LOAD;
    }

    *entry_out = ehdr->e_entry;
    printf("elf_load: OK  entry=0x%x  segments=%d\n",
           ehdr->e_entry, loaded);
    return ELF_OK;
}
