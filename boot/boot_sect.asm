[org 0x7c00]
KERNEL_OFFSET equ 0x1000 

    mov [BOOT_DRIVE], dl 
    mov bp, 0x9000
    mov sp, bp

    mov bx, MSG_REAL_MODE 
    call print
    call print_nl

    call load_kernel 
    jmp KERNEL_OFFSET

%include "./boot/boot_sect_print.asm"
%include "./boot/boot_sect_print_hex.asm"
%include "./boot/boot_sect_disk.asm"

[bits 16]
load_kernel:
    mov bx, MSG_LOAD_KERNEL1
    call print
    call print_nl

    mov bx, KERNEL_OFFSET 
    mov dh, 8
    mov dl, [BOOT_DRIVE]
    call disk_load
    mov bx, MSG_LOAD_KERNEL2
    call print
    call print_nl
    ret

BOOT_DRIVE db 0 
MSG_REAL_MODE db "Started in 16-bit Real Mode", 0
MSG_LOAD_KERNEL1 db "Loading kernel into memory 1", 0
MSG_LOAD_KERNEL2 db "Loading kernel into memory 2", 0

times 510 - ($-$$) db 0
dw 0xaa55