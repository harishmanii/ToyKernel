//
// keyboard.h: Returns an ascii character from a global memory location
//
#pragma once

#include <stdbool.h>
#include <stdint.h>
#define KEY_INFO_ADDRESS 0x1600
#define INPUT_SIZE 128


typedef struct {
    volatile uint8_t key;   /* written by keyboard ISR, read by get_key() – must be volatile */
    bool    shift;
    bool    ctrl; 
} __attribute__ ((packed)) key_info_t;

extern volatile key_info_t *key_info;
uint8_t get_key(void);
char* get();