#include <isr.h>
#include <io.h>

void isr_keyboard();
void isr_gen_prot_fault();

IDT_ENTRY _idt_table[256];

struct idt_desc {
    uint16_t size;
    uint32_t addr;
};

void initIdt() {
    addEntry(0x21, (uint32_t)(&isr_keyboard), 3, INT_32);
    addEntry(0x0D, (uint32_t)(&isr_gen_prot_fault), 3, INT_32);
    loadIdt();
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
    static char _scan_code_1_set[] = { 
        0 , 0 , '1' , '2' , 
        '3' , '4' , '5' , '6' ,  
        '7' , '8' , '9' , '0' ,  
        '-' , '=' , 0 , 0 , 'Q' ,  
        'W' , 'E' , 'R' , 'T' , 'Y' , 
        'U' , 'I' , 'O' , 'P' , '[' , ']' ,  
        0 , 0 , 'A' , 'S' , 'D' , 'F' , 'G' ,  
        'H' , 'J' , 'K' , 'L' , ';' , '\'' , '`' ,  
        0 , '\\' , 'Z' , 'X' , 'C' , 'V' , 'B' , 'N' , 'M' , 
        ',' , '.' , '/' , 0 , '*' , 0 , ' ' 
    };
    // uint8_t c = readChar();
    // uint8_t c = getScancode();
    uint8_t c = 'a';
    if (c < 0x3a)
        kputchar(_scan_code_1_set[c]);
    picEoi(1);
}
