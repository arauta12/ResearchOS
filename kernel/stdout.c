#include <stdout.h>

static uint8_t _default_foreground = WHITE;
static uint8_t _default_background = BLACK;
static uint8_t _default_cursor_color = GRAY;

static TTY_POS _cursor = {0, 0};

void moveCursorToPos(TTY_POS* pos) {
    setBackgroundColorTTY(_default_cursor_color, pos);
    _cursor = *pos;
}

TTY_POS getCursorPos() {
    return _cursor;
}

bool incrementChar() {
    TTY_POS newCursor = _cursor;
    bool shiftedUp = false;

    newCursor.col++;
    if (newCursor.col >= TTY_WIDTH) {
        newCursor.col = 0;
        newCursor.row++;

        if (newCursor.row >= TTY_HEIGHT) {
            shiftScreenUp();
            shiftedUp = true;
        }
    }

    // move cursor
    moveCursorToPos(&newCursor);
    
    return shiftedUp;
}

bool incrementTab() {
    TTY_POS newCursor = _cursor;
    bool shiftedUp = false;

    newCursor.col = ((newCursor.col >> 2) << 2) + 4;
    if (newCursor.col >= TTY_WIDTH) {
        newCursor.col -= TTY_WIDTH;
        newCursor.row++;

        if (newCursor.row >= TTY_HEIGHT) {
            shiftScreenUp();
            shiftedUp = true;
        }
    }

    // move cursor
    moveCursorToPos(&newCursor);
    
    return shiftedUp;
}

bool incrementNewline() {
    TTY_POS newCursor = _cursor;
    bool shiftedUp = false;

    newCursor.col = 0;
    newCursor.row++;

    if (newCursor.row >= TTY_HEIGHT) {
        shiftScreenUp();
        
        newCursor.row--;
        shiftedUp = true;
    }

    // move cursor
    moveCursorToPos(&newCursor);
    
    return shiftedUp;
}


bool incrementCharFromPos(TTY_POS* pos) {
    bool shiftedUp = false;

    pos->col++;
    if (pos->col >= TTY_WIDTH) {
        pos->col = 0;
        pos->row++;

        if (pos->row >= TTY_HEIGHT) {
            shiftScreenUp();
            shiftedUp = true;
        }
    }
    
    return shiftedUp;
}

bool incrementTabFromPos(TTY_POS* pos) {
    bool shiftedUp = false;

    pos->col = ((pos->col >> 2) << 2) + 4;
    if (pos->col >= TTY_WIDTH) {
        pos->col -= TTY_WIDTH;
        pos->row++;

        if (pos->row >= TTY_HEIGHT) {
            shiftScreenUp();
            shiftedUp = true;
        }
    }
    
    return shiftedUp;
}

bool incrementNewlineFromPos(TTY_POS* pos) {
    bool shiftedUp = false;

    pos->col = 0;
    pos->row++;

    if (pos->row >= TTY_HEIGHT) {
        shiftScreenUp();
        
        pos->row--;
        shiftedUp = true;
    }
    
    return shiftedUp;
}

void shiftScreenUp() {
    TTY_POS pos = {0, 0};
    TTY_POS newPos = {0, 0};
    TTY_CELL defaultCell = { '\0', (_default_background << 4) | _default_foreground};

    for (int row = 1; row < TTY_HEIGHT; row++) {
        for (int col = 0; col < TTY_WIDTH; col++) {
            pos.row = row;
            pos.col = col;

            newPos.row = row - 1;
            newPos.col = col;
            setCellTTY(getCell(&pos), &newPos);
            setCellTTY(&defaultCell, &pos);
        }
    }

    _cursor.row--;
}

uint8_t getDefaultForeground() {
    return _default_foreground;
}

uint8_t getDefaultBackground() {
    return _default_background;
}

uint8_t getDefaultCursorColor() {
    return _default_cursor_color;
}

void setDefaultBackground(uint8_t background) {
    _default_background = background;
}

void setDefaultForeground(uint8_t foreground) {
    _default_foreground = foreground;
}

void setDefaultCursorColor(uint8_t color) {
    _default_cursor_color = color;
}

bool printChar(uint8_t character) {
    bool shiftedUp = 0;
    setColorsTTY(_default_background, _default_foreground, &_cursor);
    switch (character) {
        case '\t':
            shiftedUp = incrementTab();
            break;
        case '\n':
            shiftedUp = incrementNewline();
            break;
        case ' ':
            shiftedUp = incrementChar();
            break;
        default:
            setCharTTY(character, &_cursor);
            shiftedUp = incrementChar();
    }

    return shiftedUp;
}

bool printCharFromPos(uint8_t character, TTY_POS* pos) {
    bool shiftedUp = 0;
    setColorsTTY(_default_background, _default_foreground, pos);
    switch (character) {
        case '\t':
            shiftedUp = incrementTabFromPos(pos);
            break;
        case '\n':
            shiftedUp = incrementNewlineFromPos(pos);
            break;
        case ' ':
            shiftedUp = incrementCharFromPos(pos);
            break;
        default:
            setCharTTY(character, pos);
            shiftedUp = incrementCharFromPos(pos);
    }

    return shiftedUp;
}

bool printColorChar(uint8_t character, uint8_t color) {
    bool shiftedUp = false;
    setBackgroundColorTTY(_default_background, &_cursor);
    switch (character) {
        case '\t':
            shiftedUp = incrementTab();
            break;
        case '\n':
            shiftedUp = incrementNewline();
            break;
        case ' ':
            shiftedUp = incrementChar();
            break;
        default:
            setColorCharTTY(character, color, &_cursor);
            shiftedUp = incrementChar();
    }

    return shiftedUp;
}

bool printFullColorChar(uint8_t character, uint8_t color, uint8_t background) {
    bool shiftedUp = false;
    setColorsTTY(background, color, &_cursor);
    switch (character) {
        case '\t':
            shiftedUp = incrementTab();
            break;
        case '\n':
            shiftedUp = incrementNewline();
            break;
        case '\b':
        case ' ':
            shiftedUp = incrementChar();
            break;
        default:
            setCharTTY(character, &_cursor);
            shiftedUp = incrementChar();
    }

    return shiftedUp;
}

int printString(const char* str) {
    int numShifts = 0;
    while (*str != '\0') {
        if (printChar(*str))
            numShifts++;
        str++;
    }

    return numShifts;
}

int printStringFromPos(const char* str, TTY_POS* pos) {
    TTY_POS oldPos = _cursor;
    _cursor = *pos;

    int numShifts = 0;
    while (*str != '\0') {
        if (printCharFromPos(*str, pos))
            numShifts++;
        str++;
    }

    _cursor = oldPos;
    return numShifts;
}

int printColorString(const char* str, uint8_t color) {
    int numShifts = 0;
    while (*str != '\0') {
        if (printColorChar(*str, color))
            numShifts++;
        str++;
    }

    return numShifts;
    
}

int printFullColorString(const char* str, uint8_t color, uint8_t background) {
    int numShifts = 0;
    while (*str != '\0') {
        if (printFullColorChar(*str, color, background))
            numShifts++;
        str++;
    }

    return numShifts;
}

void clearScreen() {
    TTY_POS pos = {0, 0};
    for (int row = 0; row < TTY_HEIGHT; row++) {
        for (int col = 0; col < TTY_WIDTH; col++) {
            pos.row = row;
            pos.col = col;
            setCharTTY('\0', &pos);
            setColorsTTY(_default_background, _default_foreground, &pos);
        }
    }

    _cursor.row = 0;
    _cursor.col = 0;
}
