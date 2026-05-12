#include <early_print.h>
#include <io.h>

static struct vga_info vga = {};

void disable_cursor() {
    outb(0x3D4, 0xA);
    outb(0x3D5, 0x20);
}

static inline int _get_offset(int x, int y) {
    int pos = y + x * vga.cols;
    return (pos >= vga.rows * vga.cols) ? vga.rows * vga.cols - 1 : pos;
}

int get_x_pos() { return vga.x; }

int get_y_pos() { return vga.y; }

void set_pos(int x, int y) {
    vga.x = (x < vga.rows) ? x : vga.rows - 1;
    vga.y = (y < vga.cols) ? y : vga.cols - 1;
}

void tty_init() {
    disable_cursor();
    vga.fb = (struct vga_cell*)VGA_TEXT_MEM;
    vga.x = vga.y = 0;
    vga.rows = 25;
    vga.cols = 80;
    vga.fg = WHITE;
    vga.bg = BLACK;

    tty_clear_screen();
}

COLORS tty_get_bg() { return vga.bg; }
COLORS tty_get_fg() { return vga.fg; }

void tty_clear_screen() {
    for (int i = 0; i < vga.rows * vga.cols; i++) {
        vga.fb[i].c = '\0';
        vga.fb[i].color = VGA_COLOR(vga.fg, vga.bg);
        vga.x = vga.y = 0;
    }
}

void tty_scroll_down() {
    // Shift all contents up a row
    int pos = vga.cols;
    while (pos < vga.rows * vga.cols) {
        vga.fb[pos - vga.cols] = vga.fb[pos];
        pos++;
    }

    vga.x = (vga.x > 0) ? vga.x - 1 : 0;

    // Clear last row
    pos = (vga.rows - 1) * vga.cols;
    for (int col = 0; col < vga.cols; col++) {
        vga.fb[pos].c = '\0';
        vga.fb[pos++].color = VGA_COLOR(vga.fg, vga.bg);
    }
}

int tty_put_char(char c) {
    int pos = _get_offset(vga.x, vga.y);
    vga.fb[pos].color = VGA_COLOR(vga.fg, vga.bg);
    vga.fb[pos].c = c;
    return 1;
}

int tty_put_color_char(char c, COLORS fg) {
    int pos = _get_offset(vga.x, vga.y);
    vga.fb[pos].c = c;
    vga.fb[pos].color = VGA_COLOR(fg, vga.bg);
    return 1;
}

void tty_set_fg(COLORS fg) { vga.fg = fg; }

void tty_set_bg(COLORS bg) { vga.bg = bg; }

int tty_print_string(const char* str) {
    size_t i = 0;
    int pos = _get_offset(vga.x, vga.y);
    while (str[i] != '\0') {
        char c = str[i];
        vga.fb[pos].color = VGA_COLOR(vga.fg, vga.bg);
        switch (c) {
            case '\r':
            case '\n':
                pos = (pos - pos % vga.cols) + vga.cols;
                break;
            case '\t':
                pos = (pos + 4);
                break;

            default:
                vga.fb[pos].c = c;
                pos++;
        }

        i++;
        if (pos >= vga.rows * vga.cols) {
            tty_scroll_down();
            pos -= vga.cols;
        }
    };

    set_pos(pos / vga.cols, pos % vga.cols);
    return i;
}

int tty_print_color_string(const char* str, COLORS fg) {
    size_t i = 0;
    int pos = _get_offset(vga.x, vga.y);
    while (str[i] != '\0') {
        char c = str[i];
        vga.fb[pos].color = VGA_COLOR(fg, vga.bg);
        switch (c) {
            case '\r':
            case '\n':
                pos = (pos - pos % vga.cols) + vga.cols;
                break;
            case '\t':
                pos = (pos + 3) & (~3);
                break;

            default:
                vga.fb[pos].c = c;
                pos++;
        }

        i++;
        if (pos >= vga.rows * vga.cols) {
            tty_scroll_down();
            pos -= vga.cols;
        }
    };

    set_pos(pos / vga.cols, pos % vga.cols);
    return i;
}
