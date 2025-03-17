print_msg:
    call print_char
    ret

print_char:
    lodsb
    cmp al, 0
    je done
    mov ah, 0x0e
    int 0x10
    jmp print_char

done:
    ret