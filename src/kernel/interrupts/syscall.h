#pragma once

#include "../include/stdio.h"
#include "../mm/handler/malloc.h"
#include "exceptions.h"
#include "pic.h"
#include "../drivers/display/screen.h"


void syscall_test0(void);
void syscall_test1(void);
void syscall_sleep(void);
void syscall_print(void);
__attribute__ ((naked)) void syscall_dispatcher(int_frame_32_t *frame);