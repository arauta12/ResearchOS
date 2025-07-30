#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#include <string.h>
#include <conversion.h>
#include <stdout.h>
#include <device/tty.h>

#define KERN_EMERG      0
#define KERN_ALERT      1
#define KERN_CRIT       2
#define KERN_ERR        3
#define KERN_WARNING    4
#define KERN_NOTICE     5
#define KERN_INFO       6
#define KERN_DEBUG      7

int kerror(int, const char*, ...);
int kprintf(const char*, ...);
int kputchar(int);

#endif