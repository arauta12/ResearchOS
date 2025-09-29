#include <device/disk.h>

static uint32_t _sectors_per_page = 8;
static disk_funct_st _disk_functions = { NULL, NULL };

bool readPages(void* addr, uint32_t startPage, uint16_t numPages) {
    return _disk_functions.read(addr, startPage * _sectors_per_page, numPages * _sectors_per_page);
}

bool writePages(void* addr, uint32_t startPage, uint16_t numPages) {
    return _disk_functions.write(addr, startPage * _sectors_per_page, numPages * _sectors_per_page);
}

bool diskIrq() {
    return ataCmdIrq();
}

bool setupDisk() {
    if (!ataInitialize()) {
        kerror(KERN_DEBUG, "Disk Init failed!\n");
        return false;
    }

    _sectors_per_page = PAGE_SIZE / (ataGetSectorSizeWords() * 2);
    _disk_functions.read = ataRead;
    _disk_functions.write = ataWrite;
    return true;
}
