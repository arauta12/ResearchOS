#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include <boot/boot.h>
#include <mm/meminfo.h>

typedef struct heap_entry heap_entry_st;

struct heap_entry {
    uint32_t tag;
    heap_entry_st* next;
    heap_entry_st* prev;
    uint32_t reserved;
};

static heap_entry_st* _krnl_heap_start;
static void* _knrl_heap_boundary;

static uint32_t _get_alloc_size(heap_entry_st* entry);
static uint32_t _get_dword_aligned_size(uint32_t bytes);
static void _do_free_merge(heap_entry_st* entry);

void* kmalloc(uint32_t bytes);
void kfree(void* addr);

void dumpKHeapMap();

#endif
