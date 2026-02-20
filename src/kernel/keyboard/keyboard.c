#include "keyboard.h"

key_info_t *key_info = (key_info_t *)KEY_INFO_ADDRESS;



uint8_t get_key(void)
{
    uint8_t output;

    key_info->key = 0;
    
    while (!key_info->key) 
        __asm__ __volatile__("hlt");

    output = key_info->key;
    key_info->key = 0;

    return output;
}