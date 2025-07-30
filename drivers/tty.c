#include <device/tty.h>
#include <stddef.h>

/**
 * Gets the TTY cell corresponding to the specified position
 *
 * @param pos TTY_POS pointer to 2D screen position
 * @return TTY_CELL pointer at that position
 */
static TTY_CELL* _pos_to_ptr(TTY_POS* pos) {
    return TTY_PTR + (TTY_WIDTH * pos->row) + pos->col;
}

/**
 * Gets tty cell struct from position struct
 * Return NULL if invalid
 *
 * @param pos TTY_POS pointer to 2D screen position
 * @return TTY_CELL to the screen cell in that position
 */
TTY_CELL* getCell(TTY_POS* pos) {
    if (pos == NULL)
        return NULL;

    if (pos->row < 0 || pos->row >= TTY_HEIGHT)
        return NULL;

    if (pos->col < 0 || pos->col >= TTY_WIDTH)
        return NULL;

    return _pos_to_ptr(pos);
}

/**
 * Places the character in the specified position
 * The background & foreground color remains unchanged 
 *
 * @param character The ASCII character to place at the position
 * @param pos TTY_POS pointer to 2D screen position
 */
void setCharTTY(uint8_t character, TTY_POS* pos) {
    _pos_to_ptr(pos)->character = character;
}

/**
 * Sets the background color of the specified position
 * The character & foreground color remains unchanged 
 *
 * @param background The background color (ideally a TTY_COLOR) to set
 * @param pos TTY_POS pointer to 2D screen position
 */
void setBackgroundColorTTY(uint8_t background, TTY_POS* pos) {
    _pos_to_ptr(pos)->color &= 0x0f;
    _pos_to_ptr(pos)->color |= (background << 4);
}

/**
 * Sets the foreground color of the specified position
 * The character & background color remains unchanged 
 *
 * @param foreground The foreground color (ideally a TTY_COLOR) to set
 * @param pos TTY_POS pointer to 2D screen position
 */
void setForegroundColorTTY(uint8_t foreground, TTY_POS* pos) {
    _pos_to_ptr(pos)->color &= 0xf0;
    _pos_to_ptr(pos)->color |= foreground;
}

/**
 * Sets the foreground & background color of the specified position
 * The character remains unchanged 
 *
 * @param background The background color (ideally a TTY_COLOR) to set
 * @param foreground The foreground color (ideally a TTY_COLOR) to set
 * @param pos TTY_POS pointer to 2D screen position
 */
void setColorsTTY(uint8_t background, uint8_t foreground, TTY_POS* pos) {
    _pos_to_ptr(pos)->color = (background << 4) | foreground;
}

/**
 * Places a character with a color at the specified position
 * The background remains unchanged
 *
 * @param character The ASCII character to place
 * @param foreground The foreground color (ideally a TTY_COLOR) to set
 * @param pos TTY_POS pointer to 2D screen position
 */
void setColorCharTTY(uint8_t character, uint8_t foreground, TTY_POS* pos) {
    _pos_to_ptr(pos)->character = character;
    
    _pos_to_ptr(pos)->color &= 0xf0;
    _pos_to_ptr(pos)->color |= foreground;
}

/**
 * Sets the TTY cell at the specified position to the new cell
 * NOTE: cell is NOT modified, the cell at pos is modified
 *
 * @param cell The TTY_CELL pointer to the new cell information
 * @param pos TTY_POS pointer to 2D screen position
 */
void setCellTTY(TTY_CELL* cell, TTY_POS* pos) {
    *(_pos_to_ptr(pos)) = *cell;
}

/**
 * Sets the character to the null byte for all screen cells 
 * Colors in each cell remain unchanged
 */
void clearAllCharsTTY() {
    TTY_POS pos = {0, 0};
    for (int i = 0; i < TTY_HEIGHT; i++) {
        for (int j = 0; j < TTY_WIDTH; j++) {
            pos.row = i;
            pos.col = j;
            setCharTTY('\0', &pos);
        }
    }
}

/**
 * Sets the background color to all screen cells
 * The character & foreground colors remain unchanged 
 *
 * @param background The background color (ideally a TTY_COLOR) to set for all cells
 */
void setScreenBackgroundTTY(uint8_t background) {
    TTY_POS pos = {0, 0};
    for (int i = 0; i < TTY_HEIGHT; i++) {
        for (int j = 0; j < TTY_WIDTH; j++) {
            pos.row = i;
            pos.col = j;
            setBackgroundColorTTY(background, &pos);
        }
    }
}

/**
 * Sets the foreground color to all screen cells
 * The character & background colors remain unchanged 
 *
 * @param foreground The foreground color (ideally a TTY_COLOR) to set for all cells
 */
void setScreenForegroundTTY(uint8_t foreground) {
    TTY_POS pos = {0, 0};
    for (int i = 0; i < TTY_HEIGHT; i++) {
        for (int j = 0; j < TTY_WIDTH; j++) {
            pos.row = i;
            pos.col = j;
            setForegroundColorTTY(foreground, &pos);
        }
    }
}

