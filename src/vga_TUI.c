#include "../include/vga_TUI.h"

void clearScrn() {
    char *vid = (char*)VID_MEM;
    for (int x = 0; x < VID_WIDTH; ++x) {
        for (int y = 0; y < VID_HEIGHT; ++y) {
            *vid = ' ';
            vid += 2;
        }
    }
}

void setScrnColor(unsigned char color) {
    char *vid = (char*)VID_MEM + 1;
    for (int x = 0; x < VID_WIDTH; ++x) {
        for (int y = 0; y < VID_HEIGHT; ++y) {
            *vid &= 0x0f;
            *vid |= (color << 4);
            vid += 2;
        }
    }
}

void printColorChar(
        unsigned char c, 
        unsigned char foreground,
        unsigned char background,
        unsigned int x, 
        unsigned int y) 
{
    if (x > VID_WIDTH) return;
    if (y > VID_HEIGHT) return;
    if (c == '\n') return;

    char *vid = (char*)VID_MEM + 2 * (x + VID_WIDTH * y);
    *vid = c;
    *(vid + 1) = (background << 4) | (foreground);
}

void printChar(unsigned char c, unsigned int x, unsigned int y) {
    printColorChar(c, WHITE, BLACK, x, y);
}

void printString(char *str, 
        unsigned char foreground,
        unsigned char background,
        unsigned int x, 
        unsigned int y) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == '\n' || x >= VID_WIDTH) {
            x = 0;
            y++;
        } else {
            printColorChar(str[i], foreground, background, x++, y);   
        }
        i++;
    }
}
