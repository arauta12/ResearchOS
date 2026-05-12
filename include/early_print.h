#pragma once

#include <common.h>
#include <stdbool.h>
#include <stddef.h>

#define VGA_TEXT_MEM 0xB8000
#define VGA_COLOR(fg, bg) (fg | (bg << 4))

typedef enum {
    BLACK = 0,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    GRAY,
    DARK_GRAY,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LIGHT_CYAN,
    LIGHT_RED,
    LIGH_MAGENTA,
    YELLOW,
    WHITE
} COLORS;

struct vga_cell {
    char c;
    u8 color;
};

struct vga_info {
    struct vga_cell* fb;
    int x;
    int y;
    int cols;
    int rows;
    COLORS fg;
    COLORS bg;
};

void disable_cursor();
void set_pos(int x, int y);
int get_x_pos();
int get_y_pos();
void tty_init();

int tty_put_char(char c);
int tty_put_color_char(char c, COLORS fg);
void tty_set_fg(COLORS fg);
void tty_set_bg(COLORS bg);

void tty_clear_screen();
void tty_scroll_down();

COLORS tty_get_bg();
COLORS tty_get_fg();

int tty_print_string(const char* str);
int tty_print_color_string(const char* str, COLORS fg);
