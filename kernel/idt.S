    .text
    .code32
    .globl  loadIdt
    .type   loadIdt,@function
loadIdt:
    
    lidt    idt_desc
    sti
    ret

    .globl  isr_gen_prot_fault
    .type   isr_gen_prot_fault,@function
    .extern handleGenProtFault
isr_gen_prot_fault:
    pushal
    call    handleGenProtFault
    popal
    iretl
    .extern handleKeypress
    .globl  isr_keyboard
    .type   isr_keyboard,@function
isr_keyboard:
    pushal
    call    handleKeypress
    popal
    iretl

    .data
    .extern _idt_table
idt_desc:
    .word   2047
    .long   _idt_table