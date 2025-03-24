extern idt
extern isr_keyboard_handler
global isr_keyboard
global loadIdt

idtDesc:
    dw 2048
    dd idt

isr_keyboard:
    pushad
    call isr_keyboard_handler
    popad
    iretd

loadIdt:
    lidt [idtDesc]
    sti
    ret