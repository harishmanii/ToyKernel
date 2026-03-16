#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "io.h"
#include "idt.h"
#include "../include/stdio.h"
#include "../keyboard/keyboard.h"
#include "../scheduler/task.h"


#define IRQ0_SLEEP_TIMER_TICKS_AREA 0x1800
#define RTC_DATETIME_AREA 0x1610

extern uint32_t sleep_timer_ticks ;


#define PIC_1_CMD  0x20     // input port
#define PIC_1_DATA 0x21     // output port

#define PIC_2_CMD  0xA0     // slave input port
#define PIC_2_DATA 0xA1     // slave output port



#define NEW_IRQ_0  0x20 // IRQ 0-7  will be mapped to interrupts 0x20-0x27 (32-39)
#define NEW_IRQ_8  0x28 // IRQ 8-15 will be mapped to interrupts 0x28-0x2F (40-47)

#define PIC_EOI    0x20 // "End of interrupt" command

#define PS2_DATA_PORT 0x60      // remapped keyboard controller
#define RTC_DATETIME_AREA 0x1610


void send_pic_eoi(uint8_t irq);
void disable_pic(void);
void set_irq_mask(uint8_t irq);
void clear_irq_mask(uint8_t irq);
void remap_pic(void);

__attribute__ ((interrupt)) void timer_irq0_handler(int_frame_32_t *frame);
__attribute__ ((interrupt)) void keyboard_irq1_handler(int_frame_32_t *frame);
void set_pit_channel_mode_frequency(const uint8_t channel, const uint8_t operating_mode, const uint16_t divisor);



typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} __attribute__ ((packed)) datetime_t;

extern datetime_t *datetime ;

// CMOS registers
enum {
    cmos_address = 0x70,
    cmos_data = 0x71 
};

enum{
    STATUS_REGISTER_A = 0x0A,
    STATUS_REGISTER_B = 0x0B,
    STATUS_REGISTER_C = 0x0C,
    STATUS_REGISTER_D = 0x0D
};

enum{
    RTC_SECONDS = 0x00,
    RTC_MINUTES = 0x02,
    RTC_HOURS = 0x04,
    RTC_DAY = 0x07,
    RTC_MONTH = 0x08,
    RTC_YEAR = 0x09
};


bool cmos_update_in_progress(void);
uint8_t get_rtc_register(uint8_t reg);
void enable_rtc(void);
void disable_rtc(void);

 void cmos_rtc_irq8_handler (int_frame_32_t *frame);