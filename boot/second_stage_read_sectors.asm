load_sectors:
pusha
; ax contains num to read
mov cx, 0x0a
mov bx, 0x1000
loop:
push ax     ; Push num sectors read
push cx

mov ax, 0x0201
mov dl, [BOOT_DRIVE]
mov dh, 0x0
mov es, bx          ; Save to es
mov bx, 0           ; Clear bx

int 0x3
jc disk_error

cmp al, 1
jne sectors_error

pop cx
mov ax, cx
and ax, 63  ; Extract sector
cmp ax, 63
je new_cylinder
inc ax
and cx, 0xC
or cx, ax   ; Store incremented sector
jmp done_cx_inc

new_cylinder:
mov al, cl   ; Save upper bits
shr al, 6
mov cl, ch
mov ch, al
inc cx
mov al, ch
mov ch, cl
shl al, 6
mov cl, al

done_cx_inc:
mov bx, es ; Save the es value again
add bx, 0x20 ; Increment address to read by 512/16
pop ax      ; Pop num sectors read
dec ax
cmp ax, 0
jg loop

popa
ret

disk_error:
    mov bx, DISK_ERROR
    call print
    call print_nl
    mov dh, ah 
    call print_hex 
    jmp disk_loop

sectors_error:
    mov bx, SECTORS_ERROR
    call print

disk_loop:
    jmp $

DISK_ERROR: db "Disk read error", 0
SECTORS_ERROR: db "Incorrect number of sectors read", 0