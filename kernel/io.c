#include <io.h>

/**
 * Does a time delay by writing to an (supposed) unused port.
 * @return none
 */
void io_wait() {
    outb(UNUSED_PORT, 0);
}

/**
 * Reads a byte from the specified port.
 *
 * If the reading failed (for some reason) the data is 0.
 * @param port 2 byte value represent the port number
 * @return The byte read from the specified port
 */
uint8_t inb(uint16_t port) {
    uint8_t result = 0;
    __asm__ volatile("inb %1, %0"
                     : "=a"(result)
                     : "Nd"(port));
    return result;
}

/**
 * Reads a word (2 bytes) from the specified port.
 *
 * If the reading failed (for some reason) the data is 0.
 * @param port 2 byte value represent the port number
 * @return The word read from the specified port
 */
uint16_t inw(uint16_t port) {
    uint16_t result = 0;
    __asm__ volatile("inw %1, %0"
                     : "=a"(result)
                     : "Nd"(port));
    return result;
}

/**
 * Reads a double word (4 bytes) from the specified port.
 *
 * If the reading failed (for some reason) the data is 0.
 * @param port word value representing the port number
 * @return The double word read from the specified port
 */
uint32_t inl(uint16_t port) {
    uint32_t result = 0;
    __asm__ volatile("inl %1, %0"
                     : "=a"(result)
                     : "Nd"(port));
    return result;
}

/**
 * Sends data to the specified IO port.
 * @param port word value representing the port number
 * @param data byte value to send via the IO port
 * @return none
 */
void outb(uint16_t port, uint8_t data) {
    __asm__ volatile("outb %0, %1" ::"a"(data), "Nd"(port));
}

/**
 * Sends data to the specified IO port.
 * @param port word value representing the port number
 * @param data word value to send via the IO port
 * @return none
 */
void outw(uint16_t port, uint16_t data) {
    __asm__ volatile("outw %0, %1" ::"a"(data), "Nd"(port));
}

/**
 * Sends data to the specified IO port.
 * @param port word value representing the port number
 * @param data double word value to send via the IO port
 * @return none
 */
void outl(uint16_t port, uint32_t data) {
    __asm__ volatile("outl %0, %1" ::"a"(data), "Nd"(port));
}
