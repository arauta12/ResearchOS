#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#include <string.h>
#include <device/tty.h>

int kprintf(const char*, ...);
int kputchar(int);

#endif