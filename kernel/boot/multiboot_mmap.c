#include <boot/multiboot_mmap.h>
#include <mm/kheap.h>

extern const void KRNL_END;

static mmap_entry_st* entries = NULL;
static uint32_t _total_mem_size = 0;
static uint32_t _mem_map_num_entries = 0;

mmap_entry_st* getMemMap() {
    return entries;
}

uint32_t getMemMapNumEntries() {
    return _mem_map_num_entries;
}

uint32_t getTotalMemorySize() {
    return _total_mem_size;
}

void printMemMap(tag_start_st* tagPtr) {
    mmap_tag_st* mmap = (mmap_tag_st*)tagPtr;

    const uint32_t numEntries = (tagPtr->size - 16) / mmap->entry_size;
    _mem_map_num_entries = numEntries;

    mmap_entry_st* tblEntry = (mmap_entry_st*)((uint32_t*)mmap + 4);
    entries = (mmap_entry_st*)kmalloc(sizeof(mmap_entry_st) * numEntries);

    for (uint32_t i = 0; i < numEntries; i++) {
        entries[i] = tblEntry[i];
        // kprintf("Mem entry %d) addr: %x, len: %x, type: %d\n", i, (uint32_t)entries[i].base_addr, (uint32_t)entries[i].length, entries[i].type);
    }
}

void printMemSize(tag_start_st* tagPtr) {
    uint32_t* iter = (uint32_t*)tagPtr + 2;

    uint32_t memLower = *(iter++);
    uint32_t memHigher = *(iter++);

    _total_mem_size = (memLower + memHigher) * 1024;

    kprintf("\tLower mem size: %d KB\n", memLower);
    kprintf("\tHigher mem size: %d KB\n", memHigher);
}
