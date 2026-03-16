global task_entry_trampoline
global switch_task

task_entry_trampoline:
    ; xchg bx, bx
    sti
    ret

switch_task:
    ; xchg bx, bx

    mov eax, [esp+4]    ; prev
    mov edx, [esp+8]    ; next

    push ebp
    push ebx
    push esi
    push edi

    mov [eax + 12], esp     ; save the current esp to current task esp so we can get on the next round
    mov esp, [edx + 12]     ; load the next task stack addr to esp register 

    pop edi
    pop esi
    pop ebx
    pop ebp

    ret