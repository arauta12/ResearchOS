#ifndef TTY_H
#define TTY_H

#include <stdint.h>
#include <string.h>
#include <conversion.h>

#define TTY_WIDTH 80
#define TTY_HEIGHT 25

#define TTY_ADDR 0xb8000
#define TTY_PTR (TTY_CELL*)TTY_ADDR

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
    uint8_t character;
    uint8_t color;
} TTY_CELL;

typedef struct {
    int row;
    int col;
} TTY_POS;

static TTY_CELL* _pos_to_ptr(TTY_POS* pos);

TTY_CELL* getCell(TTY_POS* pos);

void setCharTTY(uint8_t character, TTY_POS* pos);
void setBackgroundColorTTY(uint8_t background, TTY_POS* pos);
void setForegroundColorTTY(uint8_t foreground, TTY_POS* pos);
void setColorsTTY(uint8_t background, uint8_t foreground, TTY_POS* pos);
void setColorCharTTY(uint8_t character, uint8_t foreground, TTY_POS* pos);
void setCellTTY(TTY_CELL* cell, TTY_POS* pos);

void clearAllCharsTTY();
void setScreenBackgroundTTY(uint8_t background);
void setScreenForegroundTTY(uint8_t foreground);

#endif
