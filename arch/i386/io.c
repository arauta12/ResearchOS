#include <io.h>

void outb(u16 port, u8 data) {
    asm volatile("outb %0, %1" ::"a"(data), "Nd"(port));
}

void outw(u16 port, u16 data) {
    asm volatile("outw %0, %1" ::"a"(data), "Nd"(port));
}

void outl(u16 port, u32 data) {
    asm volatile("outl %0, %1" ::"a"(data), "Nd"(port));
}

u8 inb(u16 port) {
    u8 res;
    asm volatile("inb %1, %0" : "=a"(res) : "Nd"(port));
    return res;
}

u16 inw(u16 port) {
    u16 res;
    asm volatile("inw %1, %0" : "=a"(res) : "Nd"(port));
    return res;
}

u32 inl(u16 port) {
    u32 res;
    asm volatile("inl %1, %0" : "=a"(res) : "Nd"(port));
    return res;
}

void io_wait() { outb(0x80, 0); }
