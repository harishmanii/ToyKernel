//
// keyboard.h: Returns an ascii character from a global memory location
//
#pragma once

#include <stdbool.h>
#include <stdint.h>
#define KEY_INFO_ADDRESS 0x1600


typedef struct {
    uint8_t key;   
    bool    shift;
    bool    ctrl; 
} __attribute__ ((packed)) key_info_t;

extern key_info_t *key_info;
uint8_t get_key(void);