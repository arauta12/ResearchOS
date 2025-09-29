#ifndef ATA_PIO_H
#define ATA_PIO_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <device/pic.h>
#include <device/ata.h>
#include <mm/pmm.h>
#include <io.h>

#define ATA_PRIMARY_PORT    0x1F0
#define ATA_PRIMARY_CTRL    0x3F6
#define ATA_DATA_PORT       ATA_PRIMARY_PORT
#define ATA_ERROR_PORT      (ATA_PRIMARY_PORT + 1)  // Read only
#define ATA_FEATURES_PORT   (ATA_PRIMARY_PORT + 1)  // Write only
#define ATA_SECT_COUNT_PORT (ATA_PRIMARY_PORT + 2)
#define ATA_LBA_LOW_PORT    (ATA_PRIMARY_PORT + 3)
#define ATA_LBA_MID_PORT    (ATA_PRIMARY_PORT + 4)
#define ATA_LBA_HI_PORT     (ATA_PRIMARY_PORT + 5)
#define ATA_DEVICE_PORT     (ATA_PRIMARY_PORT + 6)
#define ATA_STATUS_PORT     (ATA_PRIMARY_PORT + 7)  // Read only
#define ATA_CMD_PORT        (ATA_PRIMARY_PORT + 7)  // Write only

#define ATA_ALT_STATUS_PORT ATA_PRIMARY_CTRL    // Read only
#define ATA_DEV_CTRL_PORT   ATA_PRIMARY_CTRL    // Write only
#define ATA_DRIVE_ADDR_PORT (ATA_PRIMARY_CTRL + 1)

// Device register
#define ATA_DEV_BIT         (1 << 4)
#define ATA_LBA_BIT         (1 << 6)

// Device Ctrl register
#define ATA_NIEN_BIT        (1 << 1)
#define ATA_SRST_BIT        (1 << 2)
#define ATA_HOB_BIT         (1 << 7)

// Error
#define ATA_ABRT_BIT        (1 << 2)

// Status
#define ATA_ERR_BIT         (1 << 0)
#define ATA_DRQ_BIT         (1 << 3)
#define ATA_DEV_BIT         (1 << 4)
#define ATA_FAULT_BIT       (1 << 5)
#define ATA_DRDY_BIT        (1 << 6)
#define ATA_BSY_BIT         (1 << 7)

#define ATA_IDENTIFY_DEV    0xEC
#define ATA_READ_SECTS      0x20
#define ATA_READ_SECTS_EXT  0x24
#define ATA_READ_MULT       0xC4
#define ATA_READ_MULT_EXT   0x29
#define ATA_WRITE_SECTS     0x30
#define ATA_WRITE_SECTS_EXT 0x34
#define ATA_WRITE_MULT      0xC5
#define ATA_WRITE_MULT_EXT  0x39
#define ATA_FLUSH_CACHE     0xE7
#define ATA_SET_MULTIPLE    0xC6

static int _max_attempts;

bool ataPioReadIdentify(ata_info_st* ataInfo);
bool ataPioReadTransfer(void* addr, ata_info_st* ataInfo);
bool ataPioWriteTransfer(void* addr, ata_info_st* ataInfo);
bool ataPioMultReadTransfer(void* addr, ata_info_st* ataInfo);
bool ataPioMultWriteTransfer(void* addr, ata_info_st* ataInfo);

bool ataReadSectors(void* addr, uint32_t startSect, uint16_t numSectors, ata_info_st* ataInfo);
bool ataWriteSectors(void* addr, uint32_t startSect, uint16_t numSectors, ata_info_st* ataInfo);

bool ataMultReadSectors(void* addr, uint32_t startSect, uint16_t numSectors, ata_info_st* ataInfo);
bool ataMultWriteSectors(void* addr, uint32_t startSect, uint16_t numSectors, ata_info_st* ataInfo);

#endif
