#pragma once

#include <stdint.h>

typedef struct {
    uint32_t Begin_Low;
    uint32_t Begin_High;

    uint32_t Length_Low;
    uint32_t Length_High;
    
    uint32_t Type;
    uint32_t ACPI;
} MemoryRegion;

typedef struct  {
    int RegionCount;
    MemoryRegion* Regions;
} MemoryInfo;

typedef struct {
    MemoryInfo Memory;
    uint8_t BootDevice;
} BootParams;

typedef struct 
{
    uint32_t Base_Low;
    uint32_t Base_High;

    uint32_t Length_Low;
    uint32_t Length_High;

    uint32_t Type;
    uint32_t ACPI;

} E820MemoryBlock;

enum E820MemoryBlockType 
{
    E820_USABLE = 1,
    E820_RESERVED = 2,
    E820_ACPI_RECLAIMABLE = 3,
    E820_ACPI_NVS = 4,
    E820_BAD_MEMORY = 5,
};