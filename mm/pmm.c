#include <mm/pmm.h>
#include <mm/kheap.h>
#include <boot/boot.h>

static pmmap_entry_st* _pmm_blocks = NULL;
static uint32_t _total_ram = 0;

/**
 * Merges consecutive free blocks into one. Merging occurs from right to left 
 * NOTE: Entry is might be freed!
 * @param entry PMM list entry to merge into adjacent entries
 * @return None
 */
static void _merge_free_blocks(pmmap_entry_st* entry) {
    if (entry == NULL)
        return;

    pmmap_entry_st* prev = entry->prev;
    pmmap_entry_st* next = entry->next;

    // Check if we can merge with next entry
    if (next != NULL && entry->pageIndex + entry->numPages == next->pageIndex) {
        if (next->used)
            return;

        // We can! Let's do it!
        entry->numPages += next->numPages;
        if (next->next != NULL)
            next->next->prev = entry;

        entry->next = next->next;

        // Release this entry
        kfree(next);
    }

    // Check if we can merge with previous entry
    if (prev != NULL && prev->pageIndex + prev->numPages == entry->pageIndex) {
        if (prev->used)
            return;

        // Merge prev into entry, so we can keep entry
        prev->numPages += entry->numPages;
        if (entry->next != NULL)
            entry->next->prev = prev;

        prev->next = entry->next;

        // Release prev entry
        kfree(entry);
    }
}

void pmmInit() {
    _total_ram = getTotalMemorySize();

    mmap_entry_st* entries = getMemMap();
    uint32_t numEntries = getMemMapNumEntries();

    pmmap_entry_st* curr = _pmm_blocks;

    // Iterate through all available multiboot mmap entries
    for (uint32_t i = 0; i < numEntries; i++) {
        // Cannot use
        if (entries[i].base_addr + entries[i].length <= STACK_START || entries[i].type != 1)
            continue;

        // Interferes with kernel allocation, we truncate
        if (entries[i].base_addr < STACK_START) {
            uint32_t oldAddr = entries[i].base_addr;
            entries[i].base_addr = STACK_START;
            entries[i].length -= (STACK_START - oldAddr);
        }

        pmmap_entry_st* newEntry = (pmmap_entry_st*)kmalloc(sizeof(pmmap_entry_st));
        newEntry->pageIndex = ((uint32_t)entries[i].base_addr >> PAGE_EXP) + ((entries[i].base_addr % PAGE_SIZE == 0)? 0 : 1);
        newEntry->numPages = (entries[i].length >> PAGE_EXP);
        newEntry->next = NULL;
        newEntry->prev = curr;
        newEntry->used = false;
        
        if (_pmm_blocks == NULL) {
            _pmm_blocks = newEntry;
            curr = newEntry;
            continue;
        }

        curr->next = newEntry;
        curr = curr->next;
    }
}

void* pmmAllocPages(uint32_t numPages) {
    pmmap_entry_st* curr = _pmm_blocks;

    while (curr != NULL && (curr->used || (!curr->used && curr->numPages < numPages)))
        curr = curr->next;

    // No entry has enough consecutive pages
    if (curr == NULL) {
        kerror(KERN_WARNING, "PMM: Failed to find %d free pages!\n", numPages);
        return NULL;
    }

    // This entry has the exact number of pages, so we just reuse it :)
    if (curr->numPages == numPages) {
        curr->used = true;
        return (void*)(curr->pageIndex << PAGE_EXP);
    }

    // This entry has more than we need, so we split a chunk off
    pmmap_entry_st* newAlloc = (pmmap_entry_st*)kmalloc(sizeof(pmmap_entry_st));
    curr->numPages -= numPages;

    newAlloc->numPages = numPages;
    newAlloc->pageIndex = curr->pageIndex + curr->numPages;
    newAlloc->used = true;
    newAlloc->prev = curr;
    newAlloc->next = curr->next;

    if (curr->next != NULL)
        curr->next->prev = newAlloc;

    curr->next = newAlloc;

    return (void*)(newAlloc->pageIndex << PAGE_EXP);
}

bool pmmFreePages(void* addr) {
    if ((size_t)addr % PAGE_SIZE != 0) {
        kerror(KERN_INFO, "PMM Cannot free page addr %x!\n", addr);
        return false;
    }

    uint32_t pageIndex = ((size_t)addr >> PAGE_EXP);
    pmmap_entry_st* curr = _pmm_blocks;

    while (curr != NULL && curr->pageIndex != pageIndex)
        curr = curr->next;

    if (curr == NULL) {
        kerror(KERN_INFO, "PMM Could not find allocated page %d\n", pageIndex);
        return false;
    }

    curr->used = false;
    _merge_free_blocks(curr);

    return true;
}

void pmmDump() {
    pmmap_entry_st* curr = _pmm_blocks;

    while (curr != NULL) {
        kerror(KERN_DEBUG, "PMM entry: pgI=%x, numPgs=%x, used=%d\n", 
                curr->pageIndex, curr->numPages, curr->used);

        curr = curr->next;
    }
}
