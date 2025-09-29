#include <isr.h>
#include <io.h>
#include <stdout.h>
#include <conversion.h>

IDT_ENTRY _idt_table[256];
static char _key_buffer[MAX_BUFFER_LEN] = {0};
static int _buffer_i = 0;

static uint16_t _pit_count = 0;

struct idt_desc {
    uint16_t size;
    uint32_t addr;
};

void initIdt() {
    kprintf("Starting IDT config...\n");
    addEntry(0x00, (uint32_t)(&isr_divide_zero), 0, INT_32);
    addEntry(0x08, (uint32_t)(&isr_double_fault), 0, INT_32);
    addEntry(0x0A, (uint32_t)(&isr_invalid_tss), 0, INT_32);
    addEntry(0x0B, (uint32_t)(&isr_no_segment), 0, INT_32);
    addEntry(0x0C, (uint32_t)(&isr_invalid_ss), 0, INT_32);
    addEntry(0x0D, (uint32_t)(&isr_gen_prot_fault), 0, INT_32);
    addEntry(0x0E, (uint32_t)(&isr_page_fault), 0, INT_32);

    addEntry(0x20, (uint32_t)(&isr_timer), 0, INT_32);
    addEntry(0x21, (uint32_t)(&isr_keyboard), 0, INT_32);
    addEntry(0x2E, (uint32_t)(&isr_disk1), 0, INT_32);
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
    _idt_table[interruptVector].reserved = 0;
    _idt_table[interruptVector].offsetHigh = ((isrAddress & 0xffff0000) >> 16);
    _idt_table[interruptVector].flags = IDT_INIT_FLAG | IDT_PRIVILEGE_LVL(privilege) | gate;
}

void handleDoubleFault() {
    kerror(KERN_EMERG, "Double Fault!\n");
    __asm__ volatile ("hlt");
}

void handleGenProtFault() {
    kerror(KERN_EMERG, "General Protection Fault!\n");
    __asm__ volatile ("hlt");
}

void handleDivideByZero() {
    kerror(KERN_CRIT, "Divide by 0!\n");
    __asm__ volatile ("hlt");
}

void handleInvalidTSS() {
    kerror(KERN_CRIT, "Invalid TSS!\n");
    __asm__ volatile ("hlt");
}

void handleNoSegment() {
    kerror(KERN_CRIT, "Needed system segment not found!\n");
    __asm__ volatile ("hlt");
}

void handleInvalidStackSegment() {
    kerror(KERN_CRIT, "Invalid Stack Segment!\n");
    __asm__ volatile ("hlt");
}

void handlePageFault() {
    uint32_t linearAddress = 0x0;
    uint32_t errorCode = 0x0;
    __asm__ volatile ("movl %%cr2, %0" : "=r" (linearAddress));
    __asm__ volatile ("popl %0" : "=r" (errorCode));
    kerror(KERN_CRIT, "Page for %x not in memory! Error: %x\n", linearAddress, errorCode);
    __asm__ volatile ("hlt");
}

void handleTimer() {
    _pit_count = _pit_count + 1;
    reloadCounter();

    picEoi(0);
}

uint16_t getTimerCount() {
    return _pit_count;
}

uint16_t getCycleElapsed(uint16_t oldCount) {
    uint16_t pitCount = _pit_count;
    if (oldCount <= pitCount)
        return pitCount - oldCount;

    return UINT16_MAX - (oldCount - pitCount);
}

void handleKeypress() {
    key_st key = irqGetKeyboardChar();

    if (key.cmd == ESCAPE && key.pressedDown) {
        clearScreen();
        kprintf("> ");
    } 
    
    if (key.cmd == NOT_CMD && key.pressedDown) {
        if (key.data == '\n') {
            kprintf("\n");
            
            if (_buffer_i >= 5 && strncmp(_key_buffer, "greet", 5) == 0)
                kprintf("Hi! Welcome to ResearchOS\n");

            _buffer_i = 0;
            kprintf("> ");
        } else if (_buffer_i < MAX_BUFFER_LEN) {
            kputchar(key.data);
            _key_buffer[_buffer_i] = key.data;
            _buffer_i++;
        }
    }
    
    picEoi(1);
}
