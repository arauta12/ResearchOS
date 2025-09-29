#ifndef MBR_H
#define MBR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>
#include <mm/kheap.h>
#include <mm/memcpy.h>
#include <device/disk.h>
#include <boot/multiboot_info.h>

#define MAX_PARTITIONS  4

typedef struct __attribute__((packed)) {
    uint8_t bootable;
    uint8_t startChsHead;
    uint16_t startChsSectCyl;
    uint8_t sysId;
    uint8_t endChsHead;
    uint16_t endChsSectCyl;
    uint32_t partitionLbaOffset;
    uint32_t partitionLbaSize;
} partition_st;

static uint32_t diskId;
static uint32_t partitionIndex;
static partition_st* partitions;

void setBootPartition(uint32_t index);
bool readMbr();
bool isPartitionBootable(uint32_t index);

#endif
