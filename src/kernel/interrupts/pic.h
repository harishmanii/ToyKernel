#pragma once
#include <stdint.h>
#include "io.h"
#include "idt.h"
#include "../include/stdio.h"


#define PIC_1_CMD  0x20     // input port
#define PIC_1_DATA 0x21     // output port

#define PIC_2_CMD  0xA0     // slave input port
#define PIC_2_DATA 0xA1     // slave output port



#define NEW_IRQ_0  0x20 // IRQ 0-7  will be mapped to interrupts 0x20-0x27 (32-39)
#define NEW_IRQ_8  0x28 // IRQ 8-15 will be mapped to interrupts 0x28-0x2F (40-47)

#define PIC_EOI    0x20 // "End of interrupt" command

#define PS2_DATA_PORT 0x60      // remapped keyboard controller


void send_pic_eoi(uint8_t irq);
void disable_pic(void);
void set_irq_mask(uint8_t irq);
void clear_irq_mask(uint8_t irq);
void remap_pic(void);