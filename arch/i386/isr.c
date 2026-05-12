#include <arch/i386/idt.h>
#include <arch/i386/isr.h>
#include <stddef.h>

struct idt_entry _idt_table[256] = {0};

void register_idt_entry(u8 num, u32 addr, int priv_mode, GATE_TYPE type) {
    kprintf("Registering int #%x\n", num);
    _idt_table[num].offsetLow = (addr & 0x0000ffff);
    _idt_table[num].selector = 0x08;
    _idt_table[num].reserved = 0;
    _idt_table[num].offsetHigh = ((addr & 0xffff0000) >> 16);
    _idt_table[num].flags = IDT_INIT_FLAG | IDT_PRIVILEGE_LVL(priv_mode) | type;
}

void gen_prot_handler() { kerror("GEN PROT FAULT!"); }

struct idt_entry get_idt_entry(u8 num) { return _idt_table[num]; }

void enable_int() { __asm__ __volatile__("sti"); }
void disable_int() { __asm__ __volatile__("cli"); }
