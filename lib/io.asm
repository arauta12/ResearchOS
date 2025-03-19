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
ReadDisk:
    mov ah, 0x02
    ; al is input
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, 0x80
    int 0x13

    jc DiskFailure
    ret

DiskFailure:
    mov si, failure
    call Print
    jmp $

failure db "Hard disk failed to read...", 0xa, 0xd, 0
