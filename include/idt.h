#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include "ps2_driver.h"

struct IDT_ENTRY {
    uint16_t offset_l;
    uint16_t selector;
    uint8_t reserved;
    uint8_t attributes;
    uint16_t offset_h;
};

extern struct IDT_ENTRY idt[];
extern void loadIdt();
extern unsigned int isr_keyboard;

void add_entry(
    int index,
    uint32_t base_addr, 
    uint16_t selector,
    uint8_t attributes
);

void init_idt();

#endif