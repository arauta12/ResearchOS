#include <mm/kheap.h>

extern const char KRNL_END[];

static heap_entry_st* _krnl_heap_start = NULL;
static void* _knrl_heap_boundary = (void*)(KRNL_END);

/**
 * Get allocation size for the entry (all bits but bit 0).
 * Excludes the header / metadata
 * @param entry Heap entry
 * @return Number of allocated bytes for the entry
 */
static uint32_t _get_alloc_size(heap_entry_st* entry) {
    return entry->tag & 0xFFFFFFE;
}

/**
 * Number of bytes as a multiple of dwords (4 bytes).
 * Ex. If 5 bytes are requested, this return 8.
 * @param bytes Requested number of bytes
 * @return Number of bytes as a multiple of 4
 */
static uint32_t _get_dword_aligned_size(uint32_t bytes) {
    uint32_t allocSize = bytes + ((bytes % 4 != 0)? (4 - (bytes % 4)) : 0);
    return allocSize;
}

/**
 * Merges free heap entries memory adjacent to given entry (behind & ahead).
 * If two entries cannot be merged, the merge process is skipped. 
 * The merged entry also recovers bytes allocated for data and headers from the others.
 * @param entry Heap entry to merge with its neighbors
 * @return None
 */
static void _do_free_merge(heap_entry_st* entry) {
    if (entry == NULL)
        return;

    heap_entry_st* prev = entry->prev;
    heap_entry_st* next = entry->next;

    // Check if we can merge with next entry
    if (next != NULL && (uint8_t*)next == ((uint8_t*)entry + sizeof(heap_entry_st) + _get_alloc_size(entry))) {
        if (next->tag % 2 == 1)
            return;

        // We can! Let's do it!
        entry->tag += (sizeof(heap_entry_st) + _get_alloc_size(next));
        if (next->next != NULL)
            next->next->prev = entry;

        entry->next = next->next;
    }

    // Check if we can merge with previous entry
    if (prev != NULL && (uint8_t*)entry == ((uint8_t*)prev + sizeof(heap_entry_st) + _get_alloc_size(prev))) {
        if (prev->tag % 2 == 1)
            return;

        // Merge entry into prev
        prev->tag += (sizeof(heap_entry_st) + _get_alloc_size(entry));
        if (entry->next != NULL)
            entry->next->prev = prev;

        prev->next = entry->next;
    }
}

/**
 * Allocate a chunk of kernel heap memory of at least given size (in bytes).
 * 
 * NOTE: Do not assume more bytes are allocated than what was requested
 * 
 * TODO: Must revisit to check for page boundaries
 * @param bytes Number of bytes requested to be allocated
 * @return Pointer to allocated space
 */
void* kmalloc(uint32_t bytes) {
    heap_entry_st* curr = _krnl_heap_start;
    uint32_t allocSize = _get_dword_aligned_size(bytes);

    // No entry in the list
    if (_krnl_heap_start == NULL) {
        _krnl_heap_start = _knrl_heap_boundary;

        _krnl_heap_start->tag = (allocSize) | 0x1;
        _krnl_heap_start->next = NULL;
        _krnl_heap_start->prev = NULL;

        void* addr = (void*)((uint8_t*)_krnl_heap_start + sizeof(heap_entry_st));
        _knrl_heap_boundary = (void*)((uint8_t*)addr + allocSize);

        return addr;
    }

    uint32_t oldAllocSize = _get_alloc_size(curr);

    while (curr->next != NULL && ((curr->tag % 2 == 1) || (curr->tag % 2 == 0 && oldAllocSize < allocSize))) {
        curr = curr->next;
        oldAllocSize = _get_alloc_size(curr);
    }
    
    // End of list with no free entry
    if (curr->tag % 2 == 1) {
        heap_entry_st* newEntry = (heap_entry_st*)_knrl_heap_boundary;
        
        newEntry->prev = curr;
        newEntry->next = NULL;
        newEntry->tag = (allocSize) | 0x1;

        curr->next = newEntry;

        void* addr = (void*)((uint8_t*)newEntry + sizeof(heap_entry_st));
        _knrl_heap_boundary = (void*)((uint8_t*)addr + allocSize);

        return addr;
    }

    // End of list with free entry
    if (curr->next == NULL) {
        curr->tag = (allocSize) | 0x1;

        void* addr = (void*)((uint8_t*)curr + sizeof(heap_entry_st));
        _knrl_heap_boundary = (void*)((uint8_t*)addr + allocSize - oldAllocSize);

        return addr;
    }

    // Have a free entry but not at the end

    // Enough space for another entry?
    uint8_t* addr = (uint8_t*)curr;
    if (oldAllocSize - allocSize < sizeof(heap_entry_st) + 4) {
        // Not enough, just use the whole thing to prevent fragmentation
        curr->tag |= 0x1;
    } else {
        // Yes!
        curr->tag = (allocSize) | 0x1;

        heap_entry_st* newEntry = (heap_entry_st*)((uint8_t*)curr + sizeof(heap_entry_st) + allocSize);
        newEntry->tag = (oldAllocSize - allocSize - sizeof(heap_entry_st));
        newEntry->prev = curr;
        newEntry->next = curr->next;
        
        curr->next = newEntry;
    }

    return (void*)addr;
    
}

/**
 * Free up the allocation space references by the given pointer.
 * If the pointer does not correspond to an allocation, it prints a message and returns
 * early.
 * @param addr Pointer to allocation provided earlier
 * @return None
 */
void kfree(void* addr) {
    heap_entry_st* curr = _krnl_heap_start;

    while (curr != NULL && (uint8_t*)curr + sizeof(heap_entry_st) != (uint8_t*)addr)
        curr = curr->next;

    // Addr not found to be allocated
    if (curr == NULL) {
        kerror(KERN_INFO, "Addr %x is invalid!\n", addr);
        return;
    }

    // Chunk already freed!
    if (curr->tag % 2 == 0) {
        kerror(KERN_INFO, "Addr %x is already freed!\n", addr);
        return;
    }

    // Found allocation, set flag to freed
    curr->tag = _get_alloc_size(curr);
    _do_free_merge(curr);
}

/**
 * A debugging tool to print all heap entries, including allocation and usage flag
 * @return None
 */
void dumpKHeapMap() {
    heap_entry_st* curr = _krnl_heap_start;
    kerror(KERN_DEBUG, "KHEAP ENTRIES:\n");
    while (curr != NULL) {
        kerror(KERN_DEBUG, "\tAddr: %x) used?: %d, size: %x\n", 
                curr, curr->tag % 2, _get_alloc_size(curr));
        curr = curr->next;
    }
}
