#ifndef STDOUT_H
#define STDOUT_H

#include <device/tty.h>
#include <stdbool.h>

typedef struct {
    TTY_POS pos;
    uint8_t character;
    uint8_t prevBackground;
    uint8_t prevForeground;
} CELL;

static TTY_POS _cursor;

static uint8_t _default_foreground;
static uint8_t _default_background;
static uint8_t _default_cursor_color;

// Move pos functions
bool incrementChar();
bool incrementTab();
bool incrementNewline();

bool incrementCharFromPos(TTY_POS* pos);
bool incrementTabFromPos(TTY_POS* pos);
bool incrementNewlineFromPos(TTY_POS* pos);

void shiftScreenUp();

void moveCursorToPos(TTY_POS* pos);

TTY_POS getCursorPos();

// Getting default values
uint8_t getDefaultForeground();
uint8_t getDefaultBackground();
uint8_t getDefaultCursorColor();

// Set default values
void setDefaultForeground(uint8_t background);
void setDefaultBackground(uint8_t foreground);
void setDefaultCursorColor(uint8_t color);

// Print char from cursor
bool printChar(uint8_t character);
bool printCharFromPos(uint8_t character, TTY_POS* pos);
bool printColorChar(uint8_t character, uint8_t color);
bool printFullColorChar(uint8_t character, uint8_t color, uint8_t background);

// Print string from cursor
int printString(const char* str);
int printStringFromPos(const char* str, TTY_POS* pos);
int printColorString(const char* str, uint8_t color);
int printFullColorString(const char* str, uint8_t color, uint8_t background);

void clearScreen();

#endif
