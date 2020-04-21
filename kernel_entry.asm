[bits 32]
[extern entry_point]
[extern kernel_end]
[extern kernel_start]
[extern gdt_descriptor]

KERNEL_OFFSET equ 0xC0000000

section .text
global root_page_directory
global _start
_start:
mov eax, (kernel_end - KERNEL_OFFSET + 0x100 + 3) ; Get where we need to map up to
mov ebx, (virtual_page_table - KERNEL_OFFSET + 16 * 4) ; Start table entry
mov ecx, (kernel_start - KERNEL_OFFSET + 0x3) ; Start physical offset
fill_table:
    cmp ecx, kernel_start - KERNEL_OFFSET
    mov [ebx], ecx ; Write to page table entry
    add ecx, 4096
    add ebx, 4
    cmp ecx, eax
    jl fill_table
mov dword [virtual_page_table - KERNEL_OFFSET + 1023 * 4], 0x00b8003 ; Remap the video memory
mov dword [root_page_directory - KERNEL_OFFSET], (virtual_page_table - KERNEL_OFFSET + 0x03)
mov dword [root_page_directory - KERNEL_OFFSET + 768 * 4], (virtual_page_table - KERNEL_OFFSET + 0x03)
mov eax, (root_page_directory - KERNEL_OFFSET)
mov cr3, eax
mov eax, cr0
or eax, 0x80010000
mov cr0, eax
lea eax, [vm_mode]
jmp eax
vm_mode:
lgdt [gdt_descriptor] ; Load our mapped copy of the gdt 
mov dword [root_page_directory], 0x0 ; Wipe old physical mapping
mov eax, cr3
mov cr3, eax
mov esp, stack_top ; Set the stack to our temporary kernel stack
call entry_point
jmp $

section .data
align 0x1000
root_page_directory:
    times 4096 db 0
virtual_page_table:
    times 4096 db 0
    
section .bss
stack_bottom:
    resb 16384
stack_top: