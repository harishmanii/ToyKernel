[bits 32]

extern kstart                  
extern __kernel_stack_top       
global kentry

section .entry
kentry:
    mov  eax, [esp + 4]             ; eax = BootParams*
    mov  esp, __kernel_stack_top
    xor  ebp, ebp                   ; mark bottom of call chain (no previous function called before for prologue)

    push eax                       
    call kstart

    ; kstart must never return, but halt defensively just in case.
.halt:
    cli
    hlt
    jmp  .halt
