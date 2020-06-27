[extern new_proc_entry_point]
[bits 32]
%macro backup 0
    pushf
    pusha ; Push all GP registers to stack
%endmacro

; Performs a context switch
global context_switch
context_switch:
    push ebp
    mov ebp, esp
    backup
    mov eax, [ebp + 8] ; New esp to switch to
    mov ebx, [ebp + 12] ; Where to store current state
    ; Swap stack pointers
    mov [ebx+4], esp ; Save current stack pointer
    mov ecx, cr3
    mov [ebx], ecx  ; Back up page directory

    mov ecx, [eax] ; Load new page directory
    mov cr3, ecx ; Safe to overwrite since we are in kernel code with same mapping in both address spaces
    mov esp, [eax+4] ; Load new stack pointer
    ; Pop values pushed
    popa ; Pop the registers
    popf
    leave
    ret

global fork_process
fork_process:
    push ebp
    mov ebp, esp
    pusha

    mov eax, [ebp + 8] ; Child context in eax
    mov edx, [ebp + 12] ; Param for entry point

    mov ebp, esp ; Save old stack in ebp until we can free up edx
    mov ebx, cr3 ; Save our current cr3

    ; Switch to child address space
    mov ecx, [eax] ; New cr3 in ecx
    mov cr3, ecx ; Switch address spaces
    mov esp, [eax+4] ; Load new stack pointer

    push edx ; Push edx on the new stack
    mov edx, ebp ; Get the parent stack now we dont need edx

    lea ecx, [child_complete]
    push ecx ; Push the return address for the child

    push esp ; Push current stack as base pointer
    mov ebp, esp ; Set the base pointer correctly
    backup

    mov [eax+4], esp ; Save the stack pointer with saved values

    ; Child is setup for its first context switch

    ; Switch back to parent address space
    mov esp, edx
    mov cr3, ebx

    popa
    leave
    ret

child_complete:
    ; The child should have the param sitting on the stack already
    call new_proc_entry_point
    ; This should never return. If it does loop forever
    jmp $