#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "bootparams.h"

#define ASMCALL __attribute__((cdecl))

void ASMCALL x86_outb(uint16_t port, uint8_t value);
uint8_t ASMCALL x86_inb(uint16_t port);

bool ASMCALL x86_Disk_GetDriveParams(uint8_t drive,
                                                    uint8_t* driveTypeOut,
                                                    uint16_t* cylindersOut,
                                                    uint16_t* sectorsOut,
                                                    uint16_t* headsOut);

bool ASMCALL x86_Disk_Reset(uint8_t drive);

bool ASMCALL x86_Disk_Read(uint8_t drive,
                                          uint16_t cylinder,
                                          uint16_t sector,
                                          uint16_t head,
                                          uint8_t count,
                                          void* lowerDataOut);


int ASMCALL x86_E820GetNextBlock(E820MemoryBlock* block, uint32_t* continuationId);