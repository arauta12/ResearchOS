#include <mm/memcpy.h>

void memcpy(void* destination, void* src) {
    size_t* dest = (size_t*)destination;
    size_t* src32 = (size_t*)src;

    for (int i = 0; i < PAGE_SIZE / sizeof(size_t); i++)
        dest[i] = src32[i];
}

void memncpy(void* destination, void* src, size_t bytes) {
    uint8_t* destTmp = (uint8_t*)destination;
    uint8_t* srcTemp = (uint8_t*)src;

    for (int i = 0; i < bytes; i++)
        destTmp[i] = srcTemp[i];
}
