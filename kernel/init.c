#include <device/pci.h>
#include <device/pic.h>
#include <device/ps2.h>
#include <device/ps2_keyboard.h>
#include <device/tty_cursor.h>
#include <device/atapio.h>
#include <isr.h>
#include <stdio.h>
#include <stdout.h>
#include <mm/pmm.h>
#include <mm/kheap.h>
#include <mm/memcpy.h>
#include <mm/kvmm.h>
#include <mm/pg_toggle.h>

void kmain() {
    picRemap(0x20, 0x28);
    if (ps2Initiate())
        ps2KeyboardConfig();
    
    pitConfig();
    pciEnumerate();
    disableCursorTTY();
    
    ataInitialize(0);

    initIdt();
}
