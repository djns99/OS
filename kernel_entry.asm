[bits 32]
[extern entry_point]
global _start
_start:
; boot_sect.asm pushed the num sectors before jumping here
call entry_point
jmp $