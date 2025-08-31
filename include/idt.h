#ifndef IDT_H
#define IDT_H

void loadIdt();
void isr_keyboard();
void isr_disk1();
void isr_timer();
void isr_gen_prot_fault();
void isr_divide_zero();
void isr_invalid_tss();
void isr_no_segment();
void isr_invalid_ss();
void isr_page_fault();
void isr_double_fault();

#endif