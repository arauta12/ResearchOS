#include <device/tty.h>

static int currentCell = 0;
static uint8_t defaultColor = (BLACK << 4) | WHITE;

TTY_CELL* getCellPtrTTY(int cell) {
    return TTY_PTR + cell;
}

void setCharTTY(uint8_t c, int cell) {
    TTY_CELL* ptr = getCellPtrTTY(cell);
    
    if (c == '\t') {
        currentCell += 4;
    } else if (c == '\n') {
        currentCell += TTY_WIDTH - (currentCell % TTY_WIDTH); 
    } else if (c == '\b') {
        ptr->letter = '\0';
        currentCell--;
        if (currentCell < 0)
            currentCell = 0;
    } else {
        ptr->letter = c;    
    }
}

void setBackgroundColorTTY(uint8_t backColor, int cell) {
    TTY_CELL* ptr = getCellPtrTTY(cell);
    ptr->color &= 0x0f;
    ptr->color |= (backColor << 4);
}

void setForegroundColorTTY(uint8_t foreColor, int cell) { 
    TTY_CELL* ptr = getCellPtrTTY(cell);
    ptr->color &= 0xf0;
    ptr->color |= foreColor;
}

void setColorsTTY(uint8_t backColor, uint8_t foreColor, int cell) {
    TTY_CELL* ptr = getCellPtrTTY(cell);
    ptr->color = (backColor << 4) | foreColor;
}

void clearScreenTTY() {
    int cell = 0;
    while (cell < TTY_WIDTH * TTY_HEIGHT) {
        setColorsTTY(BLACK, WHITE, cell);
        setCharTTY('\0', cell);
        cell++;
    }
    
    currentCell = 0;
}

void printCharTTY(uint8_t c) {
    setCharTTY(c, currentCell++);
    currentCell %= (TTY_WIDTH * TTY_HEIGHT);
}

void printCharForegroundTTY(uint8_t foreColor, uint8_t c) {
    setCharTTY(c, currentCell);
    setForegroundColorTTY(foreColor, currentCell++);
    currentCell %= (TTY_WIDTH * TTY_HEIGHT);
}

void printCharColorTTY(uint8_t backColor, uint8_t foreColor, uint8_t c) {
    setCharTTY(c, currentCell);
    setColorsTTY(backColor, foreColor, currentCell++);
    currentCell %= (TTY_WIDTH * TTY_HEIGHT);
}

void printStringTTY(const char* str) {
    const char* ptr = str;
    while (*ptr != '\0') {
        printCharTTY(*ptr);
        ptr++;
    }
}

void printDigit(int digit) {
    printCharTTY(digit + 0x30);
}

void printInteger(int num) {
    char str[12];
    intToString(str, num);
    printStringTTY(str);
}
