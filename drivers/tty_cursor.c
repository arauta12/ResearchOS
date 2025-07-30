#include <device/tty_cursor.h>

void disableCursorTTY() {
    outb(TTY_CURSOR_PORT_1, 0x0A);
    io_wait();
    outb(TTY_CURSOR_PORT_2, 0x20);
}
