[extern kernel_end]
[extern kernel_start]

VIRT_KERNEL_OFFSET equ 0xC0000000

global load_memmap
load_memmap:
; Note must be called before virtual memory is init
pusha
mov ecx, [0x8000]
cmp ecx, MEMMAP_MAX_NUM_ENTRIES ; Check it fits in buffer
jle load
cmp ecx, 0 ; Check its nonzero
jne load
mov dword [memmap_num_entries - VIRT_KERNEL_OFFSET], 0xdeadbeef ; Set magic flag to indicate failure
jmp return
load:
mov dword [memmap_num_entries - VIRT_KERNEL_OFFSET], ecx
mov eax, MEMMAP_ENTRY_SIZE
mul ecx ; Get number of bytes in table 
mov ecx, eax
mov eax, 0x8004 ; Start from beginning of table
lea ebx, [memmap_table - VIRT_KERNEL_OFFSET] ; Copy to our local table
loop:
mov edx, dword [eax]
mov dword [ebx], edx
add eax, 4
add ebx, 4
sub ecx, 4
cmp ecx, 0
jg loop
return:
popa
ret

global get_kernel_start
get_kernel_start:
    mov eax, kernel_start
    ret
global get_kernel_end
get_kernel_end:
    mov eax, kernel_end
    ret
global get_phys_kernel_start
get_phys_kernel_start:
    mov eax, (kernel_start - VIRT_KERNEL_OFFSET)
    ret
global get_phys_kernel_end
get_phys_kernel_end:
    mov eax, (kernel_end - VIRT_KERNEL_OFFSET)
    ret


section .bss
MEMMAP_ENTRY_SIZE equ 24
MEMMAP_MAX_NUM_ENTRIES equ 256
global memmap_num_entries
global memmap_table
memmap_num_entries:
 resd 1
memmap_table:
 resb MEMMAP_ENTRY_SIZE*MEMMAP_MAX_NUM_ENTRIES ; At most 256 entries of 24 bytes each
