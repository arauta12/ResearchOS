#include <isr.h>
#include <io.h>
#include <stdio.h>
#include <device/tty.h>

void isr_keyboard();
void isr_gen_prot_fault();

IDT_ENTRY _idt_table[256];

struct idt_desc {
    uint16_t size;
    uint32_t addr;
};

void initIdt() {
    kprintf("Starting IDT config...\n");
    addEntry(0x21, (uint32_t)(&isr_keyboard), 3, INT_32);
    addEntry(0x0D, (uint32_t)(&isr_gen_prot_fault), 3, INT_32);
    loadIdt();
    kprintf("IDT config complete.\n");
}

void addHardwareEntry(uint8_t interruptVector, uint32_t isrAddress, GATE_TYPE gate) {
    addEntry(interruptVector, isrAddress, 0, gate);
}

void addEntry(uint8_t interruptVector, uint32_t isrAddress, int privilege, GATE_TYPE gate) {
    if (privilege < 0 || privilege > 3)
        return;

    _idt_table[interruptVector].offsetLow = (isrAddress & 0x0000ffff);
    _idt_table[interruptVector].selector = 0x08;
    _idt_table[interruptVector].flags = 0x8e;
    _idt_table[interruptVector].reserved = 0;
    _idt_table[interruptVector].offsetHigh = ((isrAddress & 0xffff0000) >> 16);
    // _idt_table[interruptVector].flags = IDT_INIT_FLAG | IDT_PRIVILEGE_LVL(privilege) | gate;
}

void handleGenProtFault() {
    kprintf("ERROR: General Protection Fault!\nStopping...");
    __asm__ volatile ("hlt");
}

void handleKeypress() {
    KEYCHAR key = irqGetKeyboardChar();
    if (key.letter != 0xff && key.pressedDown)
        kputchar(key.letter);
    
    picEoi(1);
}
