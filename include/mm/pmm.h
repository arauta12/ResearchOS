#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stddef.h>

#include <boot/multiboot_mmap.h>
#include <mm/kheap.h>
#include <mm/meminfo.h>

typedef struct pmmap_entry pmmap_entry_st;

struct pmmap_entry {
    uint32_t pageIndex;
    uint32_t numPages;
    bool used;
    pmmap_entry_st* next;
    pmmap_entry_st* prev;
};

static uint32_t _total_ram;
static pmmap_entry_st* _pmm_blocks;

static void _merge_free_blocks(pmmap_entry_st* entry);

void pmmInit();
void* pmmAllocPages(uint32_t numPages);
bool pmmFreePages(void*);
void pmmDump();

#endif
