#ifndef ATAPIO_H
#define ATAPIO_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <device/pic.h>
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
#define ATA_FAULT_BIT       (1 << 5)
#define ATA_DRDY_BIT        (1 << 6)
#define ATA_BSY_BIT         (1 << 7)

#define ATA_IDENTIFY_DEV    0xEC
#define ATA_READ_SECTS      0x20
#define ATA_READ_SECTS_EXT  0x24
#define ATA_WRITE_SECTS     0x30
#define ATA_WRITE_SECTS_EXT 0x34
#define ATA_FLUSH_CACHE     0xE7
#define ATA_NO_CMD          0

typedef enum {
    NO_MODE,
    CHS,
    LBA28
} ATA_ADDR_MODE;

static bool _lba_supported;
static bool _flush_supported;
static bool _flush_supported_ext;
static bool _sector_multiple_supported;

static ATA_ADDR_MODE _curr_mode;
static int _drive_selected;
static bool _int_enabled;
static uint32_t _num_usable_sectors;
static int _max_attempts;
static uint16_t _max_drq_block_sectors;
static uint16_t _drq_block_sectors;

static uint8_t _last_cmd;
static uint8_t _num_sectors_remaining;
static uint8_t _num_sectors_transferred;
static uint16_t* _curr_transfer_addr;

static bool _wait_bsy(bool bsy);
static bool _wait_drdy(bool drdy);
static bool _wait_drq(bool drq);
static bool _is_drq();
static bool _is_bsy();
static bool _has_err();
static void _wait_400ns();

int ataGetDrive();
bool ataSetDrive(bool dev1);
void ataToggleInterrupts(bool enable);
bool ataSoftwareReset();

bool ataReadSector(void* addr, uint32_t startSect, uint8_t numSectors);
bool ataWriteSectors(void* addr, uint32_t startSect, uint8_t numSectors);
bool ataSectorsRW(void* addr, uint32_t startSect, uint8_t numSectors, bool readMode);
bool ataIdentifyDevice();
bool ataInitialize(int diskNum);
void ataSectorIrq();

#endif
