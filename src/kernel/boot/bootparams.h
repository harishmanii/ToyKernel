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