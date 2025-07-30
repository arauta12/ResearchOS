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

    .extern handleTimer
    .globl  isr_timer
    .type   isr_timer,@function
isr_timer:
    pushal
    call    handleTimer
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

    .extern handleDivideByZero
    .globl  isr_divide_zero
    .type   isr_divide_zero,@function
isr_divide_zero:
    pushal
    call    handleDivideByZero
    popal
    iretl

    .extern handleInvalidTSS
    .globl  isr_invalid_tss
    .type   isr_invalid_tss,@function
isr_invalid_tss:
    pushal
    call    handleInvalidTSS
    popal
    iretl

    .extern handleNoSegment
    .globl  isr_no_segment
    .type   isr_no_segment,@function
isr_no_segment:
    pushal
    call    handleNoSegment
    popal
    iretl

    .extern handleInvalidStackSegment
    .globl  isr_invalid_ss
    .type   isr_invalid_ss,@function
isr_invalid_ss:
    pushal
    call    handleInvalidStackSegment
    popal
    iretl

    .extern handlePageFault
    .globl  isr_page_fault
    .type   isr_page_fault,@function
isr_page_fault:
    pushal
    call    handlePageFault
    popal
    iretl

    .extern handleDoubleFault
    .globl  isr_double_fault
    .type   isr_double_fault,@function
isr_double_fault:
    pushal
    call    handleDoubleFault
    popal
    iretl

    .data
    .extern _idt_table
idt_desc:
    .word   2047
    .long   _idt_table