#include <stdio.h>
#include <isr.h>
#include <device/pic.h>
#include <device/ps2.h>
#include <device/ps2_keyboard.h>

void kmain() {
    picRemap(0x20, 0x28);
    if (!ps2Initiate()) {
        kprintf("Stopping...");
        return;
    }

    if (!ps2KeyboardConfig()) {
        kprintf("Stopping...");
        return;
    }

    initIdt();
    
    clearScreenTTY();
}
