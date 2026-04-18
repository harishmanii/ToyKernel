// user land entry point
#include "../../lib/syscall.h"
#include "../../lib/stdio.h"

/* Input buffer size (characters per line) */
#define INPUT_BUF_SIZE 256

void _start(void)
{
    uint32_t prompt = 1;

    println("========================================");
    println("  MyKernel - Userland Echo Shell");
    println("  Type a line and press Enter.");
    println("========================================");

    char buf[INPUT_BUF_SIZE];

    /* Infinite input/echo loop */
    while (1) {
        printf("%u> ", prompt++);
        readline(buf, INPUT_BUF_SIZE);  /* blocks until Enter pressed */
        println("testing");
        if (buf[0] == '\0') {
            /* Empty line – just show the prompt again */
            continue;
        }

        print("Echo: ");
        println(buf);
    }
}

