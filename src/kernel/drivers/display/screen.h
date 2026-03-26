#pragma once

#include <stdint.h>
#include "x86.h"


void setAxis(uint16_t xaxiz,uint16_t yaxiz);
char getchr(int x, int y);
void putchr(int x, int y, char c);
uint8_t getcolor(int x, int y);
void putcolor(int x, int y, uint8_t color);
void setcursor(int x, int y);
void clrscr();
void scrollback();
void put_c(char c);
void put_s(const char* str);
