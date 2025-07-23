#include <io.h>

void io_wait() {
    outb(UNUSED_PORT, 0);
}

uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %1, %0"
                        : "=a"(result)
                        : "Nd" (port));
    return result;
}

uint16_t inw(uint16_t port) {
    uint16_t result;
    __asm__ volatile ("inw %1, %0"
                        : "=a"(result)
                        : "Nd" (port));
    return result;
}

uint32_t inl(uint16_t port) {
    uint32_t result;
    __asm__ volatile ("inl %1, %0"
                        : "=a"(result)
                        : "Nd" (port));
    return result;
}

void outb(uint16_t port, uint8_t data) {
    __asm__ volatile ("outb %0, %1"
                        :: "a" (data), "Nd" (port));
}

void outw(uint16_t port, uint16_t data) {
    __asm__ volatile ("outw %0, %1"
                        :: "a" (data), "Nd" (port));
}

void outl(uint16_t port, uint32_t data) {
    __asm__ volatile ("outl %0, %1"
                        :: "a" (data), "Nd" (port));
}

