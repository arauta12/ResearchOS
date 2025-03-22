GDT_START:

GDT_NULL_DESC:
    dd 0
    dd 0
GDT_CS_SEG:
    dw 0xffff       ; limit
    dw 0x0          ; base
    db 0x0          ; base
    db 0b10011010   ; access byte
    db 0b11001111   ; flags and limit
    db 0x0
GDT_DS_SEG:
    dw 0xffff
    dw 0x0
    db 0x0
    db 0b10010010
    db 0b11001111
    db 0x0

GDT_END:

GDT_DESC:
    dw GDT_END - GDT_START - 1
    dd GDT_START

CODE_SEG equ GDT_CS_SEG - GDT_START
DS_SEG equ GDT_DS_SEG - GDT_START
