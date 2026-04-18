#include "keyboard.h"

volatile key_info_t *key_info = (volatile key_info_t *)KEY_INFO_ADDRESS;



uint8_t get_key(void)
{
    uint8_t output;

    key_info->key = 0;

    __asm__ __volatile__("sti");         
    while (!key_info->key)
        __asm__ __volatile__("hlt");       
    __asm__ __volatile__("cli");          

    output = key_info->key;
    key_info->key = 0;

    return output;
}

char* get(){
    //need to work on this completely
    static char buffer[INPUT_SIZE]; //once vm implemented use heap instead of stack
    int index = 0;
    while (1) {
        char key = get_key();

        if (key == 0x0D) {  // Enter
            buffer[index] = '\0';
            break;
        }

        if (key == 0x08) {  
            if (index > 0) {
                index--;
                //TODO: need to work on this
            }
        }
        else {
            if (index < INPUT_SIZE - 1) {
                buffer[index] = key;
                index++;
                // putc(key);  // echo
            }
        }
  }
  return buffer;
}