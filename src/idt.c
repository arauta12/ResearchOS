#include "../include/idt.h"

uint32_t base_addr = (unsigned int)&isr_keyboard;
struct IDT_ENTRY idt[256];

void init_idt(){
    // NOTE: qemu corresponds keyboard interrupt as IRQ #1
    add_entry(1, base_addr, 0x08, 0x8e);

    PIC_remap();

    outportb(0x21, 0xfd);
    outportb(0xa1, 0xff);

    loadIdt();
}

void add_entry(
    int index,
    uint32_t base_addr, 
    uint16_t selector,
    uint8_t attributes
) {
    if (index >= 256) return;

    idt[index].offset_l = (base_addr & 0xffff);
    idt[index].offset_h = (base_addr >> 16) & 0xffff;
    idt[index].selector = selector;
    idt[index].reserved = 0;
    idt[index].attributes = attributes;
}
