#pragma once
#include <common.h>
#include <early_print.h>
#include <lib/conversion.h>
#include <lib/string.h>
#include <stdarg.h>

int kputchar(char c);
int kputs(const char* str);
int kprintf(const char* format, ...);
int kerror(const char* format, ...);
