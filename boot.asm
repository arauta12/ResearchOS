[org 0x7c00]
[bits 16]

start: jmp boot

%include "lib/io.asm"

boot:
    ; mov bh, 1
    ; mov bl, 0
    ; call MovCursor

    mov si, msg
    call Print

    cli

    mov ax, 0x50
    mov es, ax
    xor bx, bx
    mov al, 2
    call ReadDisk

    jmp 0x50:00 ; 0x500
    hlt

msg db "Loading from Hard drive...", 0xa, 0xd, 0

times 510-($-$$) db 0
dw 0xaa55
