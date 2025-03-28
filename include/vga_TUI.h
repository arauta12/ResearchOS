#ifndef VGA_TUI_H
#define VGA_TUI_H

#define VID_MEM 0xb8000
#define VID_WIDTH 80
#define VID_HEIGHT 25

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHT_GRAY 7
#define DARK_GRAY 8
#define LIGHT_BLUE 9
#define LIGHT_GREEN 10
#define LIGHT_CYAN 11
#define LIGHT_RED 12
#define LIGHT_MAGENTA 13
#define YELLOW 14
#define WHITE 15


void clearScrn();
void setScrnColor(unsigned char color);
unsigned char getScrnColor();
void printChar(unsigned char c);
void printColorChar(
    unsigned char c, 
    unsigned char foreground,
    unsigned char background);

void printColorStr(char *str, 
    unsigned char foreground,
    unsigned char background);

void printStr(char *str);
#endif
