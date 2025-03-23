/*
 * PIC I/O Ports
 * _C: command port
 * _D: data port
 * 1: master port
 * 2: slave port
 * See: https://wiki.osdev.org/8259_PIC
 */

#define PIC1_C 0x20
#define PIC1_D 0x21
#define PIC2_C 0xa0
#define PIC2_D 0xa1

#define ICW1_DEF 0x10
#define ICW1_ICW4 0x01
#define ICW4_x86 0x01

// See: https://wiki.osdev.org/PS/2_Keyboard
void isr32_Handler();
