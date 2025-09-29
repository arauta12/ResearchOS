#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include <device/ata.h>
#include <mm/meminfo.h>

typedef struct {
    bool (*read)(void*, uint32_t, uint16_t);
    bool (*write)(void*, uint32_t, uint16_t);
} disk_funct_st;

static uint32_t _sectors_per_page;
static disk_funct_st _disk_functions;

bool readPages(void* addr, uint32_t startPage, uint16_t numPages);
bool writePages(void* addr, uint32_t startPage, uint16_t numPages);
bool diskIrq();
bool setupDisk();

#endif
