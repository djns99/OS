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
    mov ebx, [ebp + 12] ; Where to store current stack pointer
    ; Swap stack pointers
    mov [ebx+4], esp ; Save current stack pointer
    mov ecx, cr3
    mov [ebx], ecx  ; Back up page table
    mov ecx, [eax] ; Load new page table
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
    ; Save the parent regs
    push eax
    push ebx
    push ecx
    push edx
    
    mov eax, [ebp + 8] ; Child context in eax
    mov ebx, [ebp + 12] ; Parent context in ebx
    mov edx, [ebp + 16] ; Param for entry point
    
    ; Construct a fake call stack as if parent context switched 
    push ebx ; Parent would have called context_switch( child, parent )
    push eax ; So we push eax second

    ; Push our own return address for parent  
    lea ecx, [parent_complete]
    push ecx
    
    ; NOTE This overwrites ecx
    ; Run back up logic
    backup

    ; Save cr3 and stack pointers
    mov [ebx+4], esp
    mov ecx, cr3
    mov [ebx], ecx

    ; Perform a context switch that keeps all state except stack and cr3 pointers
    mov ecx, [eax] ; New cr3 in ecx
    mov cr3, ecx ; Switch address spaces
    mov esp, [eax+4] ; Load new stack pointer
    push eax ; Child needs to call context_switch( parent, child )
    push ebx ; So push ebx second
    
    ; Context switch back, backing up all the copied state 
    call context_switch
    ; When the child gets scheduled for the first time we come here
    ; We are the child if we make it here
    ; Child doesn't have a return value
    ; Instead we jump to the entry function passing our param (pcb) we stored in edx
    push edx
    call new_proc_entry_point
    ; This should never return. If it does loop forever
    jmp $
    

; NOPs as a buffer cause I dont know how return values work 
    nop
    nop
    nop
    nop
parent_complete:
    nop
    nop
    nop
    nop
; We are the parent pop saved registers and return
    pop eax ; Fake return value
    pop eax ; Param 1
    pop eax ; Param 2

; Restore actual state
    pop edx
    pop ecx 
    pop ebx
    pop eax
; Return as normal
    leave
    ret

    