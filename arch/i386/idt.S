    .text
    .globl  load_idt
    .type   load_idt,@function
load_idt:
    lidt    idt_descr
    ret

    .extern keyboard_handler
    .globl  isr_keyboard_handler
    .type   isr_keyboard_handler,@function
isr_keyboard_handler:
    pushal
    call    keyboard_handler
    popal
    iretl

    .extern gen_prot_handler
    .globl  isr_gen_prot_handler
    .type   isr_gen_prot_handler,@function
isr_gen_prot_handler:
    jmp    gen_prot_handler
    hlt

    .data
    .extern _idt_table
idt_descr:
    .word   2047
    .long   _idt_table
