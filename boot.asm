[org 0x7c00]
[bits 16]

begin: jmp boot

%include "io.asm"
%include "gdt.asm"

boot:
    mov si, msg
    call Print

    mov ax, 0x100
    mov es, ax
    xor bx, bx
    mov al, 1
    mov cl, 2
    call ReadDisk

    mov si, sect
    call Print

    mov si, cont
    call Print

    jmp 0x100:0

msg db "Loading from Hard drive...", 0xa, 0xd, 0
sect db "Boot sector loaded successfully!", 0xa, 0xd, 0
cont db "Press any key to resume boot...", 0xa, 0xd, 0

times 510-($-$$) db 0
dw 0xaa55
