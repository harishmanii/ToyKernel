#pragma once
#include <stdint.h>

#include <stdarg.h>
#include "../keyboard/keyboard.h"
#include "../include/x86.h"

#define INPUT_SIZE 128


void clrscr();
void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint16_t count);
char* get();
void setAxis(uint16_t xaxiz,uint16_t yaxiz);