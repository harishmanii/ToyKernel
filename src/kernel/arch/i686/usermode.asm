[bits 32]

global enter_usermode

; Segment selector layout:
;   GDT[3] = user code  → index 3, RPL 3 → 3 << 3 = 24 => 0x18 | 3 = 0x1B
;   GDT[4] = user data  → index 4, 4 << 3 = 0x20 => 0x20 | 0x3 = 0x23
;
enter_usermode:
    cli                         ; no interrupts while we shuffle the stack

    mov  eax, [esp + 4]         ; eip     (first argument)
    mov  ecx, [esp + 8]         ; user_esp (second argument)

    mov  bx,  0x23              ; GDT[4] | RPL-3  (user data)
    mov  ds,  bx
    mov  es,  bx
    mov  fs,  bx
    mov  gs,  bx

    push 0x23                   ; SS  – user data selector
    push ecx                    ; ESP – user stack pointer
    pushfd                      ; EFLAGS
    or   dword [esp], 0x200     ; set IF so interrupts are enabled in ring 3
    push 0x1B                   ; CS  – user code selector
    push eax                    ; EIP – user entry point

    iret                        ; pop EIP,CS,EFLAGS,ESP,SS → ring 3
