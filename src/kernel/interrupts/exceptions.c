#include "exceptions.h"

__attribute__ ((interrupt)) void div_by_0_handler(int_frame_32_t *frame)
{
    printf("Divide by zero exception called\n ");
    frame->eip++;
}