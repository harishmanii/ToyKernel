#include "screen.h"

const unsigned int SCREEN_WIDTH = 80;
const unsigned int SCREEN_HEIGHT = 25;
const uint8_t DEFAULT_COLOR = 0x7;

uint8_t* g_ScreenBuffer = (uint8_t*)0xB8000;
int g_ScreenX = 0;
int g_ScreenY = 0;

void setAxis(uint16_t xaxiz,uint16_t yaxiz){
    g_ScreenX = xaxiz;
    g_ScreenY =yaxiz;
}

char getchr(int x, int y)
{
    return (char)g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)];
}

void putchr(int x, int y, char c)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)] = c;
}

uint8_t getcolor(int x, int y)
{
    return g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1];
}

void putcolor(int x, int y, uint8_t color)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1] = color;
}

void setcursor(int x, int y)
{
    uint16_t pos = y * SCREEN_WIDTH + x;

    x86_outb(0x3D4, 0x0F);
    x86_outb(0x3D5, (uint8_t)(pos & 0xFF));
    x86_outb(0x3D4, 0x0E);
    x86_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void clrscr()
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            putchr(x, y, 0);
            putcolor(x, y, DEFAULT_COLOR);
        }

    g_ScreenX = 0;
    g_ScreenY = 0;
    setcursor(0, 0);
}

void scrollback()
{
    for (int y = 1; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            putchr(x, y - 1, getchr(x, y));
            putcolor(x, y - 1, getcolor(x, y));
        }

    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        putchr(x, SCREEN_HEIGHT - 1, 0);
        putcolor(x, SCREEN_HEIGHT - 1, DEFAULT_COLOR);
    }

    g_ScreenY -= 1;
}

void put_c(char c)
{
    switch (c)
    {
        case '\n':  g_ScreenX = 0; g_ScreenY++;
                    break;
    
        case '\t':  for (int i = 0; i < 4 - (g_ScreenX % 4); i++)
                        put_c(' ');
                    break;

        case '\r':  g_ScreenX = 0;
                    break;

        default:    putchr(g_ScreenX, g_ScreenY, c);
                    g_ScreenX += 1;
                    break;
    }

    if (g_ScreenX >= SCREEN_WIDTH) {
        g_ScreenX -= SCREEN_WIDTH;
        g_ScreenY++;
    }

    if (g_ScreenY >= SCREEN_HEIGHT)
        scrollback();

    setcursor(g_ScreenX, g_ScreenY);
}

void put_s(const char* str)
{
    while (*str)
    {
        put_c(*str);
        ++str;
    }
}




