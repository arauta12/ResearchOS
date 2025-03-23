#include "../include/vga_TUI.h"

void start() {
    char *msg = "--- Welcome to ResearchOS! ---\nNo Copyright\nAuthor: Andrei Rauta\0";
    char *user = "User > \0";
    clearScrn();
    setScrnColor(BLACK);
    printString(msg, WHITE, BLACK, 0, 0);
}
