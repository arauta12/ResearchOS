#ifndef ISR_H
#define ISR_H

#include <stdio.h>
#include <stdint.h>
#include <device/ps2_keyboard.h>
#include <idt.h>
#include <device/pic.h>

#define IDT_PRIVILEGE_LVL(n) n << 5
#define IDT_INIT_FLAG 1 << 7

typedef struct idt_entry {
    uint16_t offsetLow;
    uint16_t selector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t offsetHigh;
} IDT_ENTRY;

typedef enum gates {
    TASK = 0x5, // NOTE offset should be 0
    INT_16 = 0x6,
    TRAP_16 = 0x7,
    INT_32 = 0xE,
    TRAP_32 = 0xF
} GATE_TYPE;

void handleKeypress();
void initIdt();
void isr_keyboard();
void addEntry(uint8_t, uint32_t, int, GATE_TYPE);
void addHardwareEntry(uint8_t, uint32_t, GATE_TYPE);

#endif