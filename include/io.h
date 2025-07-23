#ifndef IO_H
#define IO_H

#include <stdint.h>

#define UNUSED_PORT 0x80
#define BIT_POS(n) (1 << n) /* create number with a 1 in n-bit position*/

void io_wait();
uint8_t inb(uint16_t);
uint16_t inw(uint16_t);
uint32_t inl(uint16_t);

void outb(uint16_t, uint8_t);
void outw(uint16_t, uint16_t);
void outl(uint16_t, uint32_t);

#endif