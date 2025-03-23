#include <stdint.h>

#define IDT_MEM 0x20000
#define INT_GATE 0xe
#define TASK_GATE 0x5
#define TRAP_GATE 0xf

struct IDT_ENTRY {
    uint16_t offset_l;
    uint16_t selector;
    uint8_t reserved;
    uint8_t attributes;
    uint16_t offset_h;
};

void add_entry(
    uint32_t offset, 
    uint16_t selector, 
    uint8_t reserved,
    uint8_t attributes
);

void init_idt();
