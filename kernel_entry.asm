[bits 32]
[extern entry_point]
[extern _kernel_end]
[extern _kernel_start]

section .text
global root_page_directory
global _start
_start:
mov eax, (_kernel_end - 0xC0000000) ; Get where we need to map up to
mov ebx, (virtual_page_table - 0xC0000000 + 768 * 4) ; Start table entry
mov ecx, (_kernel_start - 0xC0000000) ; Start physical offset
or dword ecx, 0x3 ; Flags for page table entry
fill_table:
    mov [ebx], ecx
    add ecx, 4096
    add ebx, 4
    cmp ecx, eax
    jle fill_table
mov dword [ebx], 0x00b8003 ; Remap the video memory
mov dword [root_page_directory - 0xC0000000], (virtual_page_table - 0xC0000000 + 0x03)
mov dword [root_page_directory - 0xC0000000 + 768 * 4], (virtual_page_table - 0xC0000000 + 0x03)
mov eax, (root_page_directory - 0xC0000000)
mov cr3, eax
mov eax, cr0
or eax, 0x80010000
mov cr0, eax
lea eax, [vm_mode]
jmp [eax]
vm_mode:
mov dword [root_page_directory], 0x0 ; Wipe old physical mapping 
call entry_point
jmp $

section .data
align 0x1000
root_page_directory:
    times 4096 db 0
virtual_page_table:
    times 4096 db 0