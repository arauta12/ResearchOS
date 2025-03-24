#include "../include/vga_TUI.h"

static int current_cell = 0;
static unsigned char screenColor = BLACK;

void clearScrn() {
    char *vid = (char*)VID_MEM;
    for (int x = 0; x < VID_WIDTH; ++x) {
        for (int y = 0; y < VID_HEIGHT; ++y) {
            *vid = ' ';
            vid += 2;
        }
    }
    current_cell = 0;
}

void setScrnColor(unsigned char color) {
    screenColor = color;
    char *vid = (char*)VID_MEM + 1;
    for (int x = 0; x < VID_WIDTH; ++x) {
        for (int y = 0; y < VID_HEIGHT; ++y) {
            *vid &= 0x0f;
            *vid |= (screenColor << 4);
            vid += 2;
        }
    }
}

unsigned char getScrnColor() { return screenColor; }

void printColorChar(
        unsigned char c, 
        unsigned char foreground,
        unsigned char background) {
    char *vid = (char*)VID_MEM + 2 * current_cell;

    *(vid + 1) = (background << 4) | (foreground);

    if (c == '\n') {
        current_cell -= (current_cell % VID_WIDTH);
        current_cell += VID_WIDTH;
        return;
    } else if (c == '\t') {
        current_cell += 4;
        return;
    }

    *vid = c;
    current_cell++;
    current_cell %= (VID_WIDTH * VID_HEIGHT);
    *(vid + 3) = (LIGHT_GRAY << 4);
}

void printChar(unsigned char c) {
    printColorChar(c, WHITE, screenColor);
}

void printColorStr(char *str, 
        unsigned char foreground,
        unsigned char background) {
    int i = 0;
    while (str[i] != '\0') {
        printColorChar(str[i++], foreground, background);
    }
}

void printStr(char *str) {
    unsigned char foreground = (screenColor == BLACK) ? WHITE : BLACK;
    printColorStr(str, foreground, screenColor);
}
