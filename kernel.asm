[org 0x500]
[bits 16]

start: jmp kernel

%include "lib/io.asm"

kernel:
    mov si, msg
    call Print
    hlt
    jmp $

msg db "Sectors loaded. Hello from Kernel!", 0xa, 0xd, 0

times 510-($-$$) db 0
dw 0xaa55