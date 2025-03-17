GDT_START:

GDT_NULL_DESC:
    dd 0x0
    dd 0x0
GDT_CS_SEG:
    dw 0xffff
    dw 0x0
    db 0x0
    db 0x9a
    db 0xcf
    db 0x0
GDT_DS_SEG:
    dw 0xffff
    dw 0x0
    db 0x0
    db 0x92
    db 0xcf
    db 0x0

GDT_END:

GDT_DESC:
    dw GDT_END - GDT_START - 1
    dd GDT_START
CS_SEG equ GDT_CS_SEG - GDT_START
DS_SEG equ GDT_DS_SEG - GDT_START