    .file "demo.S"
    .globl  _start
    .text
_start:
    call    main
    movl    %eax,   %edi
    call    exit

    .globl  write
write:
    movl    $1,     %eax
    syscall
    ret

    .globl  exit
exit:
    movl    $60,    %eax
    syscall

    .data
msg:
    .string "Hello\nWorld!"
msg_end:
len = msg_end - msg
