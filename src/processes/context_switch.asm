; Performs a context switch in user space without being in an interrupt context
global context_switch
context_switch:
    push ss
    pushf
    push cs
    pusha ; Push all GP registers to stack
    mov ax, ds
    push eax
    mov eax, [esp + 4 + 13 * 4] ; New esp to switch to
    mov ebx, [esp + 8 + 13 * 4] ; Where to store current stack pointer
    ; Swap stack pointers
    mov [ebx], esp ; Save current stack pointer
    mov ecx, cr3
    mov [ebx+4], ecx  ; Back up page table
    mov ecx, [eax+4] ; Load new page table
    mov cr3, ecx ; Safe to overwrite since we are in kernel code with same mapping in both address spaces
    mov esp, [eax] ; Load new stack pointer
    ; Pop values pushed
    pop eax 
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa ; Pop the registers
    pop cs
    popf
    pop ss
    ret
    
     
    

    