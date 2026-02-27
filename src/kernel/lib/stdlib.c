#include "../include/stdlib.h"

int str_to_int(char *str)
{
    int result = 0;

    while (*str)
    {
        result = result * 10 + (*str - '0');
        str++;
    }

    return result;
}


// Allocate uninitialized memory, uses syscall
void *malloc(const uint32_t size)
{
    void *ptr = 0;
    // TODO: Don't hardcode
    __asm__ __volatile__ ("int $0x80" : : "a"(3), "b"(size) );

    __asm__ __volatile__ ("movl %%EAX, %0" : "=r"(ptr) ); 

    return ptr;
}

// Free allocated memory at a pointer, uses syscall
void free(const void *ptr)
{
    // TODO: Don't hardcode
    __asm__ __volatile__ ("int $0x80" : : "a"(4), "b"(ptr) );
}

