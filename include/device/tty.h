#ifndef TTY_H
#define TTY_H

#include <stdint.h>
#include <string.h>
#include <conversion.h>

#define TTY_WIDTH 80
#define TTY_HEIGHT 25

typedef enum ttyColor {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    PURPLE = 5,
    BROWN = 6,
    GRAY = 7,
    DARK_GRAY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_PURPLE = 13,
    YELLOW = 14,
    WHITE = 15
} TTY_COLOR;

typedef struct {
    char letter;
    uint8_t color;
} TTY_CELL;

#define TTY_ADDR 0xb8000
#define TTY_PTR (TTY_CELL*)0xb8000

TTY_CELL* getCellPtrTTY(int cell);

void setCharTTY(uint8_t, int);
void setBackgroundColorTTY(uint8_t, int);
void setForegroundColorTTY(uint8_t, int);
void setColorsTTY(uint8_t, uint8_t, int);

void clearScreenTTY();

void printCharTTY(uint8_t);
void printStringTTY(const char*);
void printCharForegroundTTY(uint8_t, uint8_t);
void printCharColorTTY(uint8_t, uint8_t, uint8_t);

void printDigit(int);
void printInteger(int);

#endif
