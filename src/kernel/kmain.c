
#include "boot/bootparams.h"
#include "hal/hal.h"
#include "include/stdio.h"
#include "include/stdlib.h"
#include <stdint.h>
#include "include/time.h"

/*
    Refefernces are taken from OSwiki,brokenthorn,nanobyte YT and Queso Fuego YT
*/


void __attribute__((section(".entry"))) kstart(BootParams *bootParams) {
  HAL_Initialize(bootParams);
  printf("Hello world\ntesting heap allocator \n");
  // while(1){
  // printf("Enter your name : ");

  // char* name = get();
  // printf("\nYour name is %s\n",name);
  // }


//   __asm__ __volatile__ (
//     "movl $0, %eax\n\t"
//     "int $0x80"
// );
// printf("s\n");
// sleep(3);
// printf("e\n");

int arr[] = {1,2,3,4,5,0,0,0};

// int *ptr = (int *) malloc(3);
// ptr[0] = 1;
// ptr[1] = 2;
// ptr[2] = 3;

// int a = 1/0;

//here after the allocation of ptr2 the ptr value is missed need to check and debug this
// int *ptr2 = (int *) malloc(6);
// ptr2[0] = 4;
// ptr2[1] = 5;
// ptr2[2] = 6;


no_exit();
}
