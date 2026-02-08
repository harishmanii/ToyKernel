#pragma once

#include "../include/stdio.h"
#include "exceptions.h"


void syscall_test0(void);
void syscall_test1(void);
__attribute__ ((naked)) void syscall_dispatcher(int_frame_32_t *frame);