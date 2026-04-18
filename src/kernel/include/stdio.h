#pragma once
#include <stdint.h>

#include <stdarg.h>
#include "../keyboard/keyboard.h"
#include "../include/x86.h"

#define INPUT_SIZE 128

void write(char c);
void putc(char c);
void puts(const char* str);
void clrscr();
void printf(const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint16_t count);
void setAxis(uint16_t xaxiz,uint16_t yaxiz);