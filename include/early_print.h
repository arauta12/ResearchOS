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
} VGA_COLORS;

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
    VGA_COLORS fg;
    VGA_COLORS bg;
};

void disable_cursor();
void set_pos(int x, int y);
void tty_init();
int tty_put_char(char c);
int tty_put_color_char(char c, VGA_COLORS fg);
void tty_set_fg(VGA_COLORS fg);
void tty_set_bg(VGA_COLORS bg);

void tty_clear_screen();
void tty_scroll_down();

int tty_print_string(const char* str);
int tty_print_color_string(const char* str, VGA_COLORS fg);
