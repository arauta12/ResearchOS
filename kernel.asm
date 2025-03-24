[org 0x1000]
[bits 16]

kernel_start equ 0x2000

begin: jmp kernel

%include "io.asm"
%include "gdt.asm"

kernel:
    ; mov ah, 0x01
    ; int 0x16
    ; jz kernel
    
    mov ax, 0x200
    mov es, ax
    xor bx, bx
    mov al, 10
    mov cl, 3
    call ReadDisk

    mov si, prot
    call Print

    mov ah, 0x01
    mov cx, 0x2607
    int 0x10

    ; mov dl, 0
    ; mov dh, dl
    ; call SetCursor
    
    ; Start switch to Protected Mode
    cli
    lgdt [GDT_DESC]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:ld_seg

[bits 32]
ld_seg:
    mov ax, DS_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov gs, ax
    mov fs, ax

    mov ebp, 0x90000
    mov esp, ebp

    call [kernel_start + 0x18]
    jmp $

prot db "Setting up kernel...", 0xa, 0xd, 0
times 512-($-$$) db 0
