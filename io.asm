; @ret: bh = row, bl = column
GetCursorPos:
    mov ah, 0x03
    mov bh, 0
    int 0x10
    ret

; bh: move y coord 
; bl: move x coord
MovCursor:
    mov al, bh
    call GetCursorPos
    mov bh, al

    add dh, bh
    add dl, bl
    call SetCursor
    ret

; dh: y coord
; dl: x coord
SetCursor:
    mov ah, 2
    mov bh, 0
    int 0x10
    ret

; al: char to print
; bl: text color
; cx: times to repeat char
PutChar:
    mov ah, 0x09
    mov bh, 0
    int 0x10
    ret

done:
    ret

; si: zero terminated string
Print:
    lodsb
    cmp al, 0
    je done
    mov ah, 0x0e
    int 0x10
    jmp Print

; al: number of sectors to read
; al is input

; ch: cylinder
; cl: sector
; dh: head
; dl: drive #
ReadDisk:
    mov ah, 0x2
    mov ch, 0
    mov dh, 0
    mov dl, 0x81    ; change to 0x80 when qemu testing
    int 0x13

    jc DriveFailure
    ret

ReadFloppy:
    mov ah, 0x2
    mov ch, 0
    mov dh, 0
    mov dl, 0
    int 0x13

    jc BootFailure
    ret

DriveFailure:
    mov dl, al
    mov si, failure
    call Print
    mov al, dl
    call ReadFloppy
    ret

BootFailure:
    mov si, bootFail
    call Print
    jmp $

failure db "Hard disk failed to read sectors.", 0xa, 0xd, 0
readFlpy db "Reading from floppy...", 0xa, 0xd, 0
bootFail db "ERROR: Could not load in sectors.", 0xa, 0xd, 0
