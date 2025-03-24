#include "../include/vga_TUI.h"
#include "../include/idt.h"

void start() {
    char *msg = "--- Welcome to ResearchOS! ---\nNo Copyright\nAuthor: Andrei Rauta\n\n\0";
    char *user = "User > \0";
    clearScrn();
    printStr(msg);
    init_idt();
    printColorStr(user, GREEN, getScrnColor());
}
