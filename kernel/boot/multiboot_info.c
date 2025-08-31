#include <boot/multiboot_info.h>
#include <boot/multiboot_mmap.h>

static bootinfo_head_st* _multiboot_tbl_ptr = NULL;
static uint32_t _krnl_physical_addr = 0;

void readPhyAddr(tag_start_st* tagPtr) {
    uint32_t* iter = (uint32_t*)tagPtr + 2;
    _krnl_physical_addr = *iter;

    kprintf("Krnl base addr: %x\n", _krnl_physical_addr);
}

void readBootDev(tag_start_st* tagPtr) {
    uint32_t* iter = (uint32_t*)tagPtr + 2;

    uint32_t devCode = *(iter++);
    uint32_t partitionIndex = *(iter++);
    uint32_t subPartitionIndex = *iter;

    updateBootInfo(devCode, partitionIndex, subPartitionIndex);
    
    kprintf("Bios device code: %x\n", devCode);
    kprintf("Partition #: %x\n", partitionIndex);
}

void readBootCmd(tag_start_st* tagPtr) {
    uint8_t* iter = (uint8_t*)tagPtr + 8;

    const char* name = (char*)(iter);
    kprintf("Cmd line: %s\n", name);
}

void readBootName(tag_start_st* tagPtr) {
    uint8_t* iter = (uint8_t*)(tagPtr) + 8;

    const char* name = (char*)(iter);
    kprintf("Booted by: %s\n", name);
}


void readBootTable(const void* tblPtr) {
    _multiboot_tbl_ptr = (bootinfo_head_st*)tblPtr;

    kprintf("Total size: %d\n", _multiboot_tbl_ptr->total_size);
    kprintf("Reading tags from %x:\n", _multiboot_tbl_ptr);
    
    // NOTE: tags always start at 8-byte aligned address
    tag_start_st* currentTag = (tag_start_st*)tblPtr + 1;
    
    while (currentTag->type != END_TAG_TYPE && currentTag->size != 8) {
        switch (currentTag->type) {
            case BOOT_CMD_LINE:
                // readBootCmd(currentTag);
                break;
            case BOOTLOADER_NAME:
                // readBootName(currentTag);
                break;
            case BASIC_MEM:
                printMemSize(currentTag);
                break;
            case BIOS_DEVICE:
                readBootDev(currentTag);
                break;
            case MMAP:
                printMemMap(currentTag);
                break;
            case BASE_PHY_ADDR:
                readPhyAddr(currentTag);
                break;
        }

        uint32_t size = currentTag->size;
        size += ((8 - size) % 8);
        currentTag = (tag_start_st*)((uint8_t*)currentTag + size);

        while (currentTag->type >= 22 || currentTag->type < 0)
            currentTag++;
    }
}

uint32_t getKrnlPhyAddr() {
    return _krnl_physical_addr;
}
