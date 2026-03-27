#include "tss.h"
#include "gdt.h"
#include "../../include/string.h"

static TSS g_TSS;

void tss_init(void)
{
    memset(&g_TSS, 0, sizeof(TSS));

    g_TSS.ss0        = i686_GDT_DATA_SEGMENT; // setting up kernel data segment
    g_TSS.iomap_base = sizeof(TSS);           // no I/O permission bitmap

    // Fill in GDT entry [5] with the TSS address and size.
    // This can't be done at compile time because &g_TSS isn't known then.
    uint32_t base  = (uint32_t)&g_TSS;
    uint32_t limit = sizeof(TSS) - 1;

    // setting TSS GDT entries
    g_GDT[5].LimitLow     = GDT_LIMIT_LOW(limit);
    g_GDT[5].BaseLow      = GDT_BASE_LOW(base);
    g_GDT[5].BaseMiddle   = GDT_BASE_MIDDLE(base);
    g_GDT[5].Access       = GDT_ACCESS_PRESENT | GDT_ACCESS_TSS_AVAILABLE_32BIT;
    g_GDT[5].FlagsLimitHi = GDT_FLAGS_LIMIT_HI(limit, 0);
    g_GDT[5].BaseHigh     = GDT_BASE_HIGH(base);

    // Reload GDT and tell the CPU where the TSS is (ltr = load task register)
    __asm__ volatile (
        "lgdt %0\n\t"
        "ltr  %1"
        :
        : "m"(g_GDTDescriptor),
          "r"((uint16_t)i686_GDT_TSS_SEGMENT)
    );
    
}

void tss_set_kernel_stack(uint32_t esp0)
{
    g_TSS.esp0 = esp0;
}
