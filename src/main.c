#include "../include/test.h"

#define VID_MEM 0xb8000
#define VID_WIDTH 80
#define VID_HEIGHT 25

void clearScrn();

void start() {
    char *msg = "Welcome to ResearchOS! No Copyright!\0";
    char *vid = (char*)VID_MEM;
    int i = 0;
    clearScrn();
    while (msg[i] != '\0') {
        *vid = msg[i++];
        vid += 2;
    }
}

void clearScrn() {
    char *vid = (char*)VID_MEM;
    for (int x = 0; x < VID_WIDTH; ++x) {
        for (int y = 0; y < VID_HEIGHT; ++y) {
            *vid = ' ';
            vid += 2;
        }
    }
}
