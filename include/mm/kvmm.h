#ifndef KVMM_H
#define KVMM_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include <mm/kheap.h>
#include <mm/pmm.h>
#include <mm/meminfo.h>

#define VIRT_ADDR_OFFSET    0x80000000
#define _TO_PM(addr)        (addr - VIRT_ADDR_OFFSET)
#define _TO_VM(addr)        (addr + VIRT_ADDR_OFFSET)
#define PG_INDEX_EXP        10
#define PG_NUM_ENTRIES      (PAGE_SIZE / 4)

typedef struct kvmm_entry kvmm_entry_st;

struct kvmm_entry {
    void* addr;
    uint32_t numPgs;
    uint16_t flags;

    kvmm_entry_st* prev;
    kvmm_entry_st* next;
};

/**
 * PAGE ENTRY
 * Bits 0-5: same as dir entry
 * Bit 6 (D): 1=pg was written to
 * Bit 8 (PAT): 1=global page
 */

/**
 * PAGE DIR ENTRY
 * Bit 0 (P): 1=present
 * Bit 1 (R/W): 1=allow writes to this region by the entry
 * Bit 2 (U/S): 1=allow user access to this entry
 * Bit 3 (PWT): 1=write thru access
 * Bit 4 (PCD): 1=cache disable
 * Bit 5 (A): used by CPU
 * Bit 7 (PS): (for 4MB pages)
 */

static kvmm_entry_st* _krnl_vmm_map;
static kvmm_entry_st* _krnl_vmm_last;

static uint32_t* _krnl_phy_pdptr;

uint16_t _get_dir_index(void* addr);
uint16_t _get_tbl_index(void* addr);
uint16_t _get_pg_offset(void* addr);
uint32_t _get_pg_index(void* addr);

kvmm_entry_st* getKvmmMap();
uint32_t* getPgDirPtr();
void kvmmInit();
void updateKrnlPgTbl();
void addMmioEntry(void* addr, uint32_t size);

#endif