#pragma once
#include <stdint.h>

// TSS (Task State Segment) - x86 requires this structure to be exactly this layout.
// We only use esp0 and ss0 - they tell the CPU which kernel stack to use
// when switching from user mode (ring 3) to kernel mode (ring 0).
// All other fields are zeroed and unused in software task switching.
typedef struct
{
    uint16_t link;   uint16_t link_h;
    uint32_t esp0;          // kernel stack pointer (updated on every task switch)
    uint16_t ss0;    uint16_t ss0_h;  // kernel stack segment
    uint32_t esp1;   uint16_t ss1;   uint16_t ss1_h;
    uint32_t esp2;   uint16_t ss2;   uint16_t ss2_h;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint16_t es;  uint16_t es_h;
    uint16_t cs;  uint16_t cs_h;
    uint16_t ss;  uint16_t ss_h;
    uint16_t ds;  uint16_t ds_h;
    uint16_t fs;  uint16_t fs_h;
    uint16_t gs;  uint16_t gs_h;
    uint16_t ldt; uint16_t ldt_h;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed)) TSS;

// Setup TSS and load it into the CPU (call after GDT is initialized)
void tss_init(void);

// Update the kernel stack pointer - call this on every task switch
void tss_set_kernel_stack(uint32_t esp0);
