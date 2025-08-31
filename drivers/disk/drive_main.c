#include <device/drive_main.h>

void updateBootInfo(uint32_t devCode, uint32_t partitionIndex, uint32_t subPartitionIndex) {
    _boot_device_code = devCode;
    _partition_index = partitionIndex;
    _sub_partition_index = subPartitionIndex;
}

bool readBlocks(void* addr, uint32_t startBlock, uint32_t numBlocks) {
    return false;
}

bool writeBlocks(void* addr, uint32_t startBlock, uint32_t numBlocks) {
    return false;
}
