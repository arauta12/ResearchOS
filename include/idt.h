#ifndef IDT_H
#define IDT_H

void loadIdt();
void isr_keyboard();
void isr_gen_prot_fault();

#endif