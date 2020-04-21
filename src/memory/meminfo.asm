global load_memmap
load_memmap:
; Note must be called before virtual memory is init
mov ecx, [0x8000]
add ecx, 4
cmp ecx, 1024
jle load
mov [memmap], 0xdeadbeef ; Set magic flag to indicate failure
ret  
load:
lea eax, memmap
mov ebx, 0x8000
loop:
mov dword [ebx], [eax]
add eax, 4
add ebx, 4
sub ecx, 1
cmp ecx, 0
jne loop
ret

section .bss
global memmap
memmap:
 resb 1024
