    .text
    .code32

    .extern _krnl_phy_pdptr
    .extern kprintf
    .globl  pg_enable
    .type   pg_enable,@function
pg_enable:
    call    getPgDirPtr
    movl    %eax, %cr3
    movl    %cr0, %eax
    orl     $0x80000001, %eax
    movl    %eax, %cr0
    ret


    .globl  pg_disable
    .type   pg_disable,@function
pg_disable:
    movl    %cr0, %eax
    andl    $0x7FFFFFFF, %eax
    movl    %eax, %cr0
    ret

    .data
msg:
    .string "Pg tbl addr %x\n"
