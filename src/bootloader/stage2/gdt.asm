[bits 32]

; void __attribute__((cdecl)) i686_GDT_Load(GDTDescriptor* descriptor, uint16_t codeSegment, uint16_t dataSegment);
global i686_GDT_Load
i686_GDT_Load:
    
    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp         ; initialize new call frame
    
    ; load gdt
    mov eax, [ebp + 8]  ;get the GDT Descriptor table 
    lgdt [eax]  ;load the GDT table 

    ; reload code segment
    mov eax, [ebp + 12] ; get the code segments
    push eax    ; push the code segment value to the stack
    push .reload_cs ; push offset to jump to
    retf    ; now far return switches CS:EIP
; here we changing the code segment completely requires a FAR control transfer (far jump/call/ret).

.reload_cs:

    ; reload data segments
    mov ax, [ebp + 16] ; get the data segment
    ;If you don't reload the segment registers, they will still hold selectors for the old GDT, which will cause GPF when accessed.
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax,
    mov ss, ax

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret