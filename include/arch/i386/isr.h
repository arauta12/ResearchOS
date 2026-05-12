#pragma once

#include <arch/i386/ps2_keyboard.h>
#include <common.h>
#include <early_kprintf.h>
#include <stdbool.h>

#define IDT_PRIVILEGE_LVL(n) (n) << 5
#define IDT_INIT_FLAG (1 << 7)

struct idt_entry {
    u16 offsetLow;
    u16 selector;
    u8 reserved;
    u8 flags;
    u16 offsetHigh;
} __attribute__((packed));

typedef enum gates {
    TASK = 0x5,  // NOTE offset should be 0
    INT_16 = 0x6,
    TRAP_16 = 0x7,
    INT_32 = 0xE,
    TRAP_32 = 0xF
} GATE_TYPE;

void gen_prot_handler();

void enable_int();
void disable_int();
void register_idt_entry(u8 num, u32 addr, int priv_mode, GATE_TYPE type);
struct idt_entry get_idt_entry(u8 num);
