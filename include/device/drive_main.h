#ifndef DRIVE_MAIN_H
#define DRIVE_MAIN_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <device/pci.h>
#include <device/pic.h>

static uint32_t _partition_index;
static uint32_t _sub_partition_index;
static uint32_t _boot_device_code;
static uint32_t _block_size;    // Size in sectors

void updateBootInfo(uint32_t devCode, uint32_t partitionIndex, uint32_t subPartitionIndex);
bool readBlocks(void* addr, uint32_t startBlock, uint32_t numBlocks);
bool writeBlocks(void* addr, uint32_t startBlock, uint32_t numBlocks);

#endif
