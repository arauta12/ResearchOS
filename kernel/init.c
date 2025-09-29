#include <device/pci.h>
#include <device/pic.h>
#include <device/ps2.h>
#include <device/ps2_keyboard.h>
#include <device/tty_cursor.h>
#include <device/ata_pio.h>
#include <device/disk.h>
#include <isr.h>
#include <stdio.h>
#include <stdout.h>
#include <mm/pmm.h>
#include <mm/kheap.h>
#include <mm/memcpy.h>
#include <mm/kvmm.h>

#include <fs/mbr.h>

void kmain() {
    // Device setup
    picRemap(0x20, 0x28);
    if (ps2Initiate())
        ps2KeyboardConfig();
    
    pitConfig();
    pciEnumerate();
    disableCursorTTY();
    setupDisk();
    
    // FS setup
    readMbr();
    
    // Start interrupts
    initIdt();

    // Print startup screen
    clearScreen();

    kprintf("  _____                               _      ____   _____ \n");
    kprintf(" |  __ \\                             | |    / __ \\ / ____|\n");
    kprintf(" | |__) |___  ___  ___  __ _ _ __ ___| |__ | |  | | (___  \n");
    kprintf(" |  _  // _ \\/ __|/ _ \\/ _` | '__/ __| '_ \\| |  | |\\___ \\ \n");
    kprintf(" | | \\ \\  __/\\__ \\  __/ (_| | | | (__| | | | |__| |____) |\n");
    kprintf(" |_|  \\_\\___||___/\\___|\\__,_|_|  \\___|_| |_|\\____/|_____/ \n\n");

    kprintf("> ");
}
