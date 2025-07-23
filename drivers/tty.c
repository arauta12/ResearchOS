#include <device/tty.h>

static int currentCell = 0;
static uint8_t defaultColor = (BLACK << 4) | WHITE;

void shiftScreenUp() {
    for (int i = 0; i < TTY_WIDTH; i++) {
        setCharTTY('\0', i);
        setColorsTTY(BLACK, WHITE, i);
    }

    for (int line = 1; line < TTY_HEIGHT - 1; line++) {
        for (int col = 0; col < TTY_WIDTH; col++) {
            TTY_CELL* cell = getCellPtrTTY(line * TTY_WIDTH + col);
            TTY_CELL* prevCell = getCellPtrTTY((line - 1) * TTY_WIDTH + col);
            *prevCell = *cell;
        }
    }

    for (int i = 0; i < TTY_WIDTH; i++) {
        TTY_CELL* cell = getCellPtrTTY((TTY_HEIGHT - 1) * TTY_WIDTH + i);
        cell->letter = '\0';
        cell->color = defaultColor;
    }
}

TTY_CELL* getCellPtrTTY(int cell) {
    return TTY_PTR + cell;
}

void setCharTTY(uint8_t c, int cell) {
    TTY_CELL* ptr = getCellPtrTTY(cell);
    
    if (c == '\t') {
        currentCell += 4;
    } else if (c == '\n') {
        currentCell += TTY_WIDTH - (currentCell % TTY_WIDTH); 
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
        setColorsTTY(BLACK, WHITE, '\0');
        cell++;
    }
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
