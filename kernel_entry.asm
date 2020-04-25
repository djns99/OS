[bits 32]
[extern entry_point]
[extern kernel_end]
[extern kernel_start]
[extern gdt_descriptor]
[extern load_memmap]

KERNEL_OFFSET equ 0xC0000000

section .text
global root_page_directory
global _start
_start:
lea eax, [load_memmap - KERNEL_OFFSET]
call eax
mov eax, (kernel_end - KERNEL_OFFSET + 0x100 + 3) ; Get where we need to map up to
mov ebx, (virtual_page_table - KERNEL_OFFSET + 16 * 4) ; Start table entry
mov ecx, (kernel_start - KERNEL_OFFSET + 0x3) ; Start physical offset
fill_table:
    cmp ecx, (video_memory - KERNEL_OFFSET + 0x3)
    jne default_map
    ; If we are the page reserved for video memory we should redirect that to video mem
    mov dword [ebx], 0x00b8003
    jmp increment
default_map:
    mov [ebx], ecx ; Write to page table entry
increment:
    add ecx, 4096
    add ebx, 4
    cmp ecx, eax
    jl fill_table
; Identity mapping
mov dword [root_page_directory - KERNEL_OFFSET], (virtual_page_table - KERNEL_OFFSET + 0x03)
; Actual mapping
mov dword [root_page_directory - KERNEL_OFFSET + 768 * 4], (virtual_page_table - KERNEL_OFFSET + 0x03)
; Last entry of page directory maps to itself
; This treats the root dir as a page table so all pages can be accessed through last 4MiB of virtual address 
mov dword [root_page_directory - KERNEL_OFFSET + 1023 * 4], (root_page_directory - KERNEL_OFFSET + 0x03)
; Set cr3 with page directory
mov eax, (root_page_directory - KERNEL_OFFSET)
mov cr3, eax
; Update cr0 to enable paging
mov eax, cr0
or eax, 0x80010000
mov cr0, eax
; Jump to the virtual address of the code 
lea eax, [vm_mode]
jmp eax
vm_mode:
 ; Load our mapped copy of the gdt
lgdt [gdt_descriptor]
; Wipe old identity mapping 
mov dword [root_page_directory], 0x0
; Reload cr3
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
align 0x1000
global video_memory
video_memory:
    resb 4096
stack_bottom:
    resb 16384
stack_top: