[org 0x7c00]
[bits 16]

; global start

start:
    mov si, input_msg
    call print_msg
    call user_press_loop
    mov si, loading_msg
    call print_msg
    jmp switch

user_press_loop:
    mov ah, 0x0
    int 0x16
    cmp al, 0
    je user_press_loop
    ret

switch:
    cli
    lgdt [GDT_DESC]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CS_SEG:kernel_init

[bits 32]
kernel_init:
    mov ax, DS_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    jmp $

%include "gdt.asm"
%include "io.asm"

input_msg db "Press any key to enter...", 0xa,0xd,0
loading_msg db "Switching to protected mode...", 0xa,0xd,0

times 510-($-$$) db 0
dw 0xaa55