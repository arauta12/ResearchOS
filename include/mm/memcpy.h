#ifndef MEMCPY_H
#define MEMCPY_H

#include <stdint.h>
#include <stdio.h>
#include <mm/meminfo.h>

void memcpy(void* destination, void* src);
void memncpy(void* destination, void* src, size_t bytes);

#endif
