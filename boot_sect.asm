[org 0x7c00]
    mov [BOOT_DRIVE], dl 
    mov bp, 0x9000
    mov sp, bp

    call load_memory_map
    call load_kernel 
    call switch_to_pm 
    jmp $ 

%include "./boot/boot_sect_disk.asm"
%include "./boot/boot_sect_print.asm"
%include "./boot/boot_sect_print_hex.asm"
%include "./boot/32bit-gdt.asm"
%include "./boot/32bit-switch.asm"
%include "./boot/memory_map.asm"

[bits 16]
load_kernel:
    mov bx, 0x1000 
    mov es, bx 
    mov bx, 0 
    mov dh, <KERNEL_IMAGE_SECTORS>
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[bits 32]
BEGIN_PM:
    jmp 0x10000 ; Jmp to kernel start

BOOT_DRIVE db 0 

times 510 - ($-$$) db 0
dw 0xaa55