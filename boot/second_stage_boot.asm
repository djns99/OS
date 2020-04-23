[org 0x1000]
nop
nop
nop
nop
nop
mov [BOOT_DRIVE], dl
nop
nop
nop
nop
nop
nop
mov ax, <KERNEL_IMAGE_SECTORS>
nop
nop
nop
nop
nop
call load_sectors
nop
nop
nop
nop
nop
; call load_memory_map
nop
nop
nop
nop
call switch_to_pm
nop
nop
nop
nop
jmp $

%include "./boot/second_stage_read_sectors.asm"
%include "./boot/boot_sect_print.asm"
%include "./boot/boot_sect_print_hex.asm"
%include "./boot/32bit-gdt.asm"
%include "./boot/32bit-switch.asm"
%include "./boot/memory_map.asm"

[bits 32]
BEGIN_PM:
    jmp 0x10000 ; Jmp to kernel start
    jmp $


MSG_LOAD_KERNEL db "Jumped 2nd stage", 0
BOOT_DRIVE db 0
times 2048 - ($-$$) db 0 ; Pad to 2KiB
