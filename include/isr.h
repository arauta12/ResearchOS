#ifndef ISR_H
#define ISR_H

#include <device/ps2_keyboard.h>
#include <device/pic.h>
#include <device/pit.h>
#include <device/disk.h>
#include <stdio.h>
#include <stdint.h>
#include <stdout.h>
#include <stdbool.h>
#include <idt.h>
#include <stddef.h>

#define IDT_PRIVILEGE_LVL(n) n << 5
#define IDT_INIT_FLAG 1 << 7
#define MAX_BUFFER_LEN 1024

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

static uint16_t _pit_count;
static char _key_buffer[MAX_BUFFER_LEN];
static int _buffer_i;

void handleKeypress();
void handleTimer();
uint16_t getTimerCount();
uint16_t getCycleElapsed(uint16_t oldCount);
void initIdt();
void addEntry(uint8_t, uint32_t, int, GATE_TYPE);
void addHardwareEntry(uint8_t, uint32_t, GATE_TYPE);

#endif