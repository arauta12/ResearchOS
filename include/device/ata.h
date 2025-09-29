#ifndef ATA_H
#define ATA_H

typedef struct ata_info ata_info_st;

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <io.h>
#include <device/ata_pio.h>

typedef enum {
    ATA_NO_CMD,
    ATA_READ_CMD,
    ATA_WRITE_CMD,
    ATA_READ_MULT_CMD,
    ATA_WRITE_MULT_CMD,
    ATA_SOFT_RESET_CMD,
    ATA_FLUSH_CACHE_CMD,
    ATA_IDENT_CMD,
    ATA_SET_MULT_CMD,
} ATA_CMD;

// Struct of setting for the device, independent of transfer method (ie. DMA or PIO)
struct ata_info {
    uint32_t lowerMaxNumSectors;            // Lower 32-bits of # of LBA sectors (just this if no 48-bit LBA)
    uint32_t upperMaxNumSectors;            // Upper 32-bits of # of LBA sectors (just this if no 48-bit LBA)
    uint32_t sectorSize;                    // Sector size in words NOT bytes
    int maxAtaVersion;                      // Max supported ATA version
    int maxUdmaMode;                        // -1 for invalid
    int currentUdmaMode;                    // -1 for invalid
    int currentDrive;                       // current drive # (0 = primary dev0, 1 = primary dev1, 2 = secondary dev0, ...)
    bool hasMultipleCmd;                    // True means support for MULTIPLE cmds
    bool hasDma;                            // Is DMA supported?
    bool hasFlushExt;                       // NOTE: regular flush should be supported (is mandatory)
    bool hasLba48;                          // Is 48-bit LBA supported?
    bool intEnabled;                        // Are interrupts enabled
    uint8_t maxMultipleSectorSize;           // max # of sectors in MULTIPLE DRQ Sector transfer
    uint8_t currentMultipleSectorSize;       // # of sectors in MULTIPLE DRQ Sector transfer
    ATA_CMD currentCmd;
};

static ata_info_st _info;
static uint16_t* _current_addr;
static uint16_t _sectors_remaining;
static int _max_attempts;

static bool _wait_cmd_ready();

bool ataWaitBsy(bool bsy);
bool ataWaitDrdy(bool drdy);
bool ataWaitDrq(bool drq);
bool ataIsDrq();
bool ataIsBsy();
bool ataHasErr();
void ataWait400ns();
uint8_t ataGetError();
uint8_t ataGetStatus();

int ataGetDrive();
bool ataSetDrive(bool dev1);

void ataToggleInterrupts(bool enable);
bool ataSoftwareReset();
bool ataInitialize();
bool ataIdentifyDevice();
uint8_t ataSetMultipleMode(uint8_t numSectors);
uint8_t ataGetMaxDrqMultSize();
uint8_t ataGetCurrDrqMultSize();
uint32_t ataGetSectorSizeWords();
bool ataCmdIrq();

bool ataRead(void* addr, uint32_t startSect, uint16_t numSectors);
bool ataWrite(void* addr, uint32_t startSect, uint16_t numSectors);

#endif
