#ifndef TTY_CURSOR_H
#define TTY_CURSOR_H

#include <device/tty.h>
#include <io.h>

#define TTY_CURSOR_PORT_1 0x3D4
#define TTY_CURSOR_PORT_2 0x3D5

void disableCursorTTY();

#endif