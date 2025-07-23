#include <stdio.h>
#include <isr.h>
#include <device/pic.h>
#include <device/ps2_keyboard.h>
#include <device/ps2.h>

void kmain() {
    picRemap(0x20, 0x28);
    kprintf("PIC remapped. \n");
    ps2Initiate();
    // initIdt();
    kprintf("IDT set!\n");
}
