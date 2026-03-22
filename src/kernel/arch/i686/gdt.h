#pragma once
#include <stdint.h>

#define i686_GDT_CODE_SEGMENT      0x08    // Ring 0 code segment
#define i686_GDT_DATA_SEGMENT      0x10    // Ring 0 data segment
#define i686_GDT_USER_CODE_SEGMENT 0x18    // Ring 3 code  (0x18 | 3 = 0x1B when used)
#define i686_GDT_USER_DATA_SEGMENT 0x20    // Ring 3 data  (0x20 | 3 = 0x23 when used)
#define i686_GDT_TSS_SEGMENT       0x28    // TSS descriptor


/* GDT table definitions with some helper macros*/
typedef struct
{
    uint16_t LimitLow;                  // limit (bits 0-15)
    uint16_t BaseLow;                   // base (bits 0-15)
    uint8_t BaseMiddle;                 // base (bits 16-23)
    uint8_t Access;                     // access
    uint8_t FlagsLimitHi;               // limit (bits 16-19) | flags
    uint8_t BaseHigh;                   // base (bits 24-31)
} __attribute__((packed)) GDTEntry;

typedef struct
{
    uint16_t Limit;                     // sizeof(gdt) - 1
    GDTEntry* Ptr;                      // address of GDT
} __attribute__((packed)) GDTDescriptor;

typedef enum
{
    GDT_ACCESS_CODE_READABLE                = 0x02,
    GDT_ACCESS_DATA_WRITEABLE               = 0x02,

    GDT_ACCESS_CODE_CONFORMING              = 0x04,
    GDT_ACCESS_DATA_DIRECTION_NORMAL        = 0x00,
    GDT_ACCESS_DATA_DIRECTION_DOWN          = 0x04,

    GDT_ACCESS_DATA_SEGMENT                 = 0x10,
    GDT_ACCESS_CODE_SEGMENT                 = 0x18,

    GDT_ACCESS_DESCRIPTOR_TSS               = 0x00,
    GDT_ACCESS_TSS_AVAILABLE_32BIT          = 0x09,  // 32-bit TSS Available (type nibble)

    GDT_ACCESS_RING0                        = 0x00,
    GDT_ACCESS_RING1                        = 0x20,
    GDT_ACCESS_RING2                        = 0x40,
    GDT_ACCESS_RING3                        = 0x60,

    GDT_ACCESS_PRESENT                      = 0x80,

} GDT_ACCESS;

typedef enum 
{
    GDT_FLAG_64BIT                          = 0x20,
    GDT_FLAG_32BIT                          = 0x40,
    GDT_FLAG_16BIT                          = 0x00,

    GDT_FLAG_GRANULARITY_1B                 = 0x00,
    GDT_FLAG_GRANULARITY_4K                 = 0x80,
} GDT_FLAGS;

// Helper macros
#define GDT_LIMIT_LOW(limit)                (limit & 0xFFFF)
#define GDT_BASE_LOW(base)                  (base & 0xFFFF)
#define GDT_BASE_MIDDLE(base)               ((base >> 16) & 0xFF)
#define GDT_FLAGS_LIMIT_HI(limit, flags)    (((limit >> 16) & 0xF) | (flags & 0xF0))
#define GDT_BASE_HIGH(base)                 ((base >> 24) & 0xFF)

#define GDT_ENTRY(base, limit, access, flags) {                     \
    GDT_LIMIT_LOW(limit),                                           \
    GDT_BASE_LOW(base),                                             \
    GDT_BASE_MIDDLE(base),                                          \
    access,                                                         \
    GDT_FLAGS_LIMIT_HI(limit, flags),                               \
    GDT_BASE_HIGH(base)                                             \
}

/* End */


// Exposed so tss_init() can patch entry [5] with the TSS base/limit at runtime
extern GDTEntry g_GDT[];
extern GDTDescriptor g_GDTDescriptor;

void i686_GDT_Initialize();