#ifndef BOOTMMAP_H
#define BOOTMMAP_H

#include <boot/multiboot_info.h>
#include <boot/boot.h>

#define AVAILABLE_RAM   1
#define ACPI_INFO       3
#define RESERVED_MEM    4
#define RAM_MODULES     5

// Type is 4, size (bytes) is 16
typedef struct {
    tag_start_st start;
    uint32_t mem_lower;
    uint32_t mem_upper;
} basic_mem_tag_st;

typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} mmap_entry_st;

typedef struct {
    tag_start_st start;
    uint32_t entry_size;
    uint32_t entry_version;
} mmap_tag_st;

static mmap_entry_st* entries;
static uint32_t _total_mem_size;
static uint32_t _mem_map_num_entries;

void printMemMap(tag_start_st* tagPtr);
void printMemSize(tag_start_st* tagPtr);
mmap_entry_st* getMemMap();
uint32_t getTotalMemorySize();
uint32_t getMemMapNumEntries();

#endif
