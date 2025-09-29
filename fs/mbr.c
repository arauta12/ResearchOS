#include <fs/mbr.h>

static uint32_t diskId = 0;
static uint32_t partitionIndex = -1;
static partition_st* partitions = NULL;

/**
 * Update partition index from Multiboot tag table
 * 
 * @param index Partition index from Multiboot
 * @return None
 */
void setBootPartition(uint32_t index) {
    partitionIndex = index;
}

/**
 * Checks if partition if bootable
 * 
 * @param index Partition index (starts from 0)
 * @return True if bootable, False if not
 */
bool isPartitionBootable(uint32_t index) {
    if (index >= MAX_PARTITIONS)
        return false;
    
    return (partitions[index].bootable & 0x80) != 0;
}

/**
 * Get MBR & partition data from sector 0 of DISK
 * @return None
 */
bool readMbr() {
    if (partitions == NULL)
        partitions = (partition_st*)kmalloc(sizeof(partition_st) * MAX_PARTITIONS);

    // Read MBR
    uint8_t* tmpBuffer = (uint8_t*)pmmAllocPages(1);
    if (!readPages(tmpBuffer, 0, 1)) {
        kerror(KERN_ERR, "Failed to read MBR!\n");
        return false;
    }

    memncpy(partitions, (uint8_t*)tmpBuffer + 0x1BE, sizeof(partition_st) * MAX_PARTITIONS);
    pmmFreePages(tmpBuffer);

    // No partition from Multiboot yet, find one manually
    if (partitionIndex == -1) {
        int i = 0;
        while (i < MAX_PARTITIONS && !isPartitionBootable(i))
            i++;
        
        if (i == MAX_PARTITIONS) {
            kerror(KERN_ERR, "No bootable partition?\n");
            return false;
        }

        partitionIndex = i;
    }

#ifdef DEBUG
    kerror(KERN_DEBUG, "Current partition start %x sectors\n", partitions[partitionIndex].partitionLbaOffset);
    kerror(KERN_DEBUG, "Sys id: %x\n", partitions[partitionIndex].sysId);
    kerror(KERN_DEBUG, "Current partition size: %x\n", partitions[partitionIndex].partitionLbaSize);
#endif

    return true;
}
