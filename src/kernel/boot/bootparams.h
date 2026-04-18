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

/* Maximum number of modules the bootloader may pass to the kernel */
#define BOOT_MAX_MODULES 8

/*
 * A single binary module loaded by the bootloader and passed to the kernel.
 * data  – physical address of the module image in low RAM
 * size  – size of the image in bytes
 * name  – original filename (null-terminated, for identification)
 */
typedef struct {
    void    *data;
    uint32_t size;
    char     name[32];
} BootModule;

typedef struct {
    MemoryInfo Memory;
    uint8_t    BootDevice;
    uint8_t    _pad[3];          /* keep ModuleCount 4-byte aligned */
    uint32_t   ModuleCount;
    BootModule Modules[BOOT_MAX_MODULES];
} BootParams;