#include "gdt.h"

// This is the place where we define the GDT here
// created this macros for easy accessisble
/* Actual GDT entry starts*/
GDTEntry g_GDT[] = {
    // NULL descriptor
    GDT_ENTRY(0, 0, 0, 0),  //As per GDT first entry should null 

    //code segment
    GDT_ENTRY(0,
              0xFFFFF,
              GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE,
              GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K),

    //data segment
    GDT_ENTRY(0,
              0xFFFFF,
              GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE,
              GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K),

};

GDTDescriptor g_GDTDescriptor = { sizeof(g_GDT) - 1, g_GDT};

/* Actual GDT entry ends*/

void __attribute__((cdecl)) i686_GDT_Load(GDTDescriptor* descriptor, uint16_t codeSegment, uint16_t dataSegment);

void i686_GDT_Initialize()
{
    i686_GDT_Load(&g_GDTDescriptor, i686_GDT_CODE_SEGMENT, i686_GDT_DATA_SEGMENT);  //load the GDT to the RAM
}