#include "../include/ps2_driver.h"

void isr_keyboard_handler() {
    handleKeypress(inportb(0x60));
    outportb(0x20, 0x20);
    outportb(0xa0, 0x20);
}

uint8_t inportb(uint16_t _port) {
    uint8_t code;
    __asm__ volatile("inb %w1, %b0"
                    : "=a" (code)
                    : "Nd" (_port)
    );
    return code;
}

void outportb(uint16_t _port, uint8_t _data) {
    __asm__ volatile("outb %b0, %w1" :
                    : "a" (_data),
                    "Nd" (_port)
    );
}

void handleKeypress(uint8_t code) {
    char scancodes[] = {
        0, 0, '1', '2', '3', '4', '5',
        '6', '7', '8', '9', '0', '-', '=',
        0, '\t', 'q', 'w', 'e', 'r', 't', 'y',
        'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's',
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
        '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
        ',', '.', '/', 0, '*', 0, ' '
    };

    if (code >= 0x3a) return;
    keyPressed(scancodes[code]);
}

void keyPressed(uint8_t key) {
    printChar(key);
    if (key == '\n') printColorStr("User > \0", GREEN, getScrnColor());
}

void PIC_remap() {
    printStr("Remapping PIC...\n\0");
    uint8_t data_a, data_b;
    data_a = inportb(PIC1_D);
    data_b = inportb(PIC2_D);

    outportb(PIC1_C, ICW1_DEF | ICW1_ICW4);
    outportb(PIC2_C, ICW1_DEF | ICW1_ICW4);

    outportb(PIC1_D, 0);
    outportb(PIC2_D, 8);

    outportb(PIC1_D, 4);
    outportb(PIC2_D, 2);

    outportb(PIC1_D, ICW4_x86);
    outportb(PIC2_D, ICW4_x86);

    outportb(PIC1_D, data_a);
    outportb(PIC2_D, data_b);

    printStr("PIC remap done.\n\0");

}
