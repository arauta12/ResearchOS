#include <device/ata.h>

// Set initial values
static ata_info_st _info = {
    0,
    0,
    512,
    -1,
    -1,
    -1,
    false,
    false,
    false,
    false,
    0,
    0,
    ATA_NO_CMD
};

static uint16_t* _current_addr = NULL;
static uint16_t _sectors_remaining = 0;
static int _max_attempts = 3000;

/**
 * Waits until BSY=DRQ=0 and DRDY=1
 * @return True if condition above is true, False if unable to wait
 */
static bool _wait_cmd_ready() {
    if (!ataWaitBsy(false) || !ataWaitDrq(false) || !ataWaitDrdy(true) || ataHasErr()) {
        uint8_t status = inb(ATA_STATUS_PORT);
        kerror(KERN_INFO, "ATA Identify: drive still not ready (status %x)\n", status);
        return false;
    }

    return true;
}

/**
 * Waits for a maximum number of iterations for the BSY bit
 * to be equal to given value
 * @param bsy True for BSY=1, False for BSY=0
 * @param wait True for unlimited attempts, False for max attempts
 * @return True if successful, False otherwise
 */
bool ataWaitBsy(bool bsy) {
    int attempts = 0;

    uint8_t statusMask = inb(ATA_ALT_STATUS_PORT) & ATA_BSY_BIT;
    while (attempts < _max_attempts) {
        if ((bsy && statusMask != 0) || (!bsy && statusMask == 0))
            return true;

        io_wait();
        statusMask = inb(ATA_ALT_STATUS_PORT) & ATA_BSY_BIT;
        attempts++;
    }

    return false;
}

/**
 * Waits for a maximum number of iterations for the DRDY bit
 * to be equal to given value
 * @param drdy True for DRDY=1, False for DRDY=0
 * @param wait True for unlimited attempts, False for max attempts
 * @return True if successful, False otherwise
 */
bool ataWaitDrdy(bool drdy) {
    int attempts = 0;

    uint8_t statusMask = inb(ATA_ALT_STATUS_PORT) & ATA_DRDY_BIT;
    while (attempts < _max_attempts) {
        if ((drdy && statusMask != 0) || (!drdy && statusMask == 0))
            return true;

        io_wait();
        statusMask = inb(ATA_ALT_STATUS_PORT) & ATA_DRDY_BIT;
        attempts++;
    }

    return false;
}

/**
 * Waits for a maximum number of iterations for the DRQ bit
 * to be equal to given value
 * @param drq True for DRQ=1, False for DRQ=0
 * @param wait True for unlimited attempts, False for max attempts
 * @return True if successful, False otherwise
 */
bool ataWaitDrq(bool drq) {
    int attempts = 0;

    uint8_t statusMask = inb(ATA_ALT_STATUS_PORT) & ATA_DRQ_BIT;
    while (attempts < _max_attempts) {
        if ((drq && statusMask != 0) || (!drq && statusMask == 0))
            return true;

        io_wait();
        statusMask = inb(ATA_ALT_STATUS_PORT) & ATA_DRQ_BIT;
        attempts++;
    }

    return false;
}

/**
 * Checks if DRQ bit is set in Alt Status register.
 * @return True if DRQ bit is set, False if cleared 
 */
bool ataIsDrq() {
    uint8_t statusMask = inb(ATA_ALT_STATUS_PORT) & (1 << 3);
    return (statusMask != 0);
}

/**
 * Checks if BSY bit is set in Alt Status register.
 * @return True if BSY bit is set, False if cleared 
 */
bool ataIsBsy() {
    uint8_t statusMask = inb(ATA_ALT_STATUS_PORT) & (1 << 7);
    return (statusMask != 0);
}

/**
 * Checks if ERR or DF (device fault) bit is set in Alt Status register.
 * @return True if either bit is set, False if both cleared 
 */
bool ataHasErr() {
    uint8_t status = inb(ATA_ALT_STATUS_PORT);
    return (status % 2 == 1) || ((status & ATA_FAULT_BIT) != 0);
}

/**
 * Waits at least 400ns to ensure drive has enough time to update the status after a register write
 * @return None
 */
void ataWait400ns() {
    for (int i = 0; i < 20; i++)
        inb(ATA_ALT_STATUS_PORT);
}

/**
 * Get Error byte from the Error port
 * @return Error byte
 */
uint8_t ataGetError() {
    return inb(ATA_ERROR_PORT);
}

/**
 * Get Status byte from the Status port
 * @return Status byte
 */
uint8_t ataGetStatus() {
    return inb(ATA_STATUS_PORT);
}

/**
 * Enable/disable interrupts for the current drive
 * TODO: Need to wait 400ns?
 * 
 * @param enable True to enable, False to disable
 * @param ataInfo ATA Information struct
 * @return None
 */
void ataToggleInterrupts(bool enable) {
    uint8_t ctrl = inb(ATA_DEV_CTRL_PORT);
    io_wait();
    
    (enable)? clearMaskIrq(14) : setMaskIrq(14);
    outb(ATA_DEV_CTRL_PORT, (ctrl & ATA_NIEN_BIT) | ((!enable) << 1));
    _info.intEnabled = enable;
}

/**
 * Gives currently selected drive (0 for Device 0, 1 for Device 1) on current channel
 * @param ataInfo ATA Information struct
 * @return 0 for Device 0 or 1 for Device 1, -1 for an error
 */
int ataGetDrive() {
    if (!ataWaitBsy(false)) {
        kerror(KERN_INFO, "ATA get drive: drive still busy\n");
        return -1;
    }

    uint8_t devData = inb(ATA_DEVICE_PORT);
    _info.currentDrive = (devData & ATA_DEV_BIT) != 0;
    return _info.currentDrive;
}

/**
 * Change current drive for current channel: False for Device 0, True for Device 1
 * @param dev1 False for Device 0, True for Device 1
 * @param ataInfo ATA Information struct
 * @return True if changed successfully, False if an issue occurred
 */
bool ataSetDrive(bool dev1) {
    if (!ataIsBsy(false) || !ataIsDrq(false)) {
        kerror(KERN_INFO, "ATA set drive: drive still busy\n");
        return false;
    }

    uint8_t devData = inb(ATA_DEVICE_PORT);
    outb(ATA_DEVICE_PORT, (devData & 0xEF) | (dev1 << 4));
    ataWait400ns();

    const int MAX_ATTEMPTS = 5;
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS && ataIsBsy()) {
        ataWait400ns();
        attempts++;
    }

    if (ataIsBsy()) {
        kerror(KERN_WARNING, "ATA Set drive: drive still busy\n");
        return false;
    }

    if (ataHasErr()) {
        kerror(KERN_WARNING, "ATA Set drive: Error! (status: %x, error: %x)\n", inb(ATA_STATUS_PORT), inb(ATA_ERROR_PORT));
        return false;
    }

    _info.currentDrive = dev1;
    return true;
}

/**
 * Do software reset and temporarily disables interrupts
 * @param ataInfo ATA Information struct
 * @return True if successful, False if failed
 */
bool ataSoftwareReset() {
    if (!ataWaitBsy(false)) {
        kerror(KERN_WARNING, "ATA Soft Reset: Drive still busy!\n");
        return false;
    }

    bool intEnabled = _info.intEnabled;
    ataToggleInterrupts(false);

    uint8_t devCtrl = inb(ATA_DEV_CTRL_PORT);
    if ((devCtrl & ATA_SRST_BIT) != 0) {
        kerror(KERN_INFO, "ATA Soft Reset: reset already in progress\n");
        return false;
    }

    _info.currentCmd = ATA_SOFT_RESET_CMD;
    outb(ATA_DEV_CTRL_PORT, devCtrl | ATA_SRST_BIT);

    // TODO: Replace with timer equivalent
    // Wait min 5 microseconds
    for (int i = 0; i < 400; i++)
        io_wait();

    outb(ATA_DEV_CTRL_PORT, devCtrl & (~ATA_SRST_BIT));

    // Wait minimum 2ms
    const int MAX_ATTEMPTS = 2;
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS && ataIsBsy()) {
        for (int i = 0; i < 140000; i++)
            io_wait();
        
        attempts++;
    }

    if (ataIsBsy()) {
        kerror(KERN_WARNING, "ATA Soft Reset: Timed out!\n");
        return false;
    }

    uint8_t status = inb(ATA_STATUS_PORT);
    _info.currentCmd = ATA_NO_CMD;
    if ((status % 2 == 1) || ((status & ATA_FAULT_BIT) != 0)) {
        kerror(KERN_WARNING, "ATA Soft Reset: Error! (status: %x, error: %x)\n", status, inb(ATA_ERROR_PORT));
        return false;
    }

    ataToggleInterrupts(intEnabled);
    return true;
}

/**
 * Do Identify device command for Non IRQ mode
 * @param ataInfo ATA Information struct
 * @return True if success, False otherwise
 */
bool ataIdentifyDevice() {
    if (!_wait_cmd_ready())
        return false;

    _info.currentCmd = ATA_IDENT_CMD;
    outb(ATA_CMD_PORT, ATA_IDENTIFY_DEV);

    // Wait a bit so we can check the status for an error
    ataWait400ns();
    
    // Now let's check the status
    if (!ataIsBsy() && ataHasErr()) {
        kerror(KERN_INFO, "ATA Read cmd: Error (code: %x)!\n", inb(ATA_ERROR_PORT));
        _info.currentCmd = ATA_NO_CMD;
        return false;
    }

    // Skip transfer if IRQ is enabled
    if (_info.intEnabled)
        return true;

    // Wait for transfer... (assuming an IO read is 30ns)
    const int maxAttempts = 100000;
    int attempts = 0;
    while (attempts < maxAttempts && ataIsBsy())
        inb(ATA_ALT_STATUS_PORT);

    if (!ataWaitBsy(false) || !ataIsDrq(true)) {
        kerror(KERN_INFO, "ATA Read cmd: Took too long (status: %x)!\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }

    return ataPioReadIdentify(&_info);
}

/**
 * Get current max number of sectors per MULTIPLE transfer
 * @return Current max in 1 byte
 */
uint8_t ataGetMaxDrqMultSize() {
    return _info.maxMultipleSectorSize;
}

/**
 * Get current number of sectors per MULTIPLE transfer
 * @return Current max in 1 byte
 */
uint8_t ataGetCurrDrqMultSize() {
    return _info.currentMultipleSectorSize;
}

/**
 * Get sector size in terms of words (2 bytes)
 * @return Sector size in words
 */
uint32_t ataGetSectorSizeWords() {
    return _info.sectorSize;
}

/**
 * Sets the number of sectors to transfer per "session" for MULTIPLE commands
 * NOTE: If number of sectors is above the max, this will set it to max NOT to given!
 * @param numSectors New number of sectors per MUTLIPLE transfer
 * @return New MULTIPLE DRQ block size in sectors 
 */
uint8_t ataSetMultipleMode(uint8_t numSectors) {
    // Check for invalid number
    if (!_info.hasMultipleCmd || numSectors == 0)
        return _info.currentMultipleSectorSize;

    if (numSectors > _info.maxMultipleSectorSize)
        numSectors = _info.maxMultipleSectorSize;

    // Check if disk is ready for a command
    if (!_wait_cmd_ready())
        return _info.currentMultipleSectorSize;

    kerror(KERN_DEBUG, "ATA Set mult!\n");
    // Command parameter
    outb(ATA_SECT_COUNT_PORT, numSectors);

    // Send command to Disk
    _info.currentCmd = ATA_SET_MULT_CMD;
    outb(ATA_CMD_PORT, ATA_SET_MULTIPLE);

    // Leave & wait for IRQ if enabled
    if (_info.intEnabled)
        return numSectors;

    // No IRQ, so we poll the alt status reg    
    int attempts = 0;
    while (attempts < _max_attempts && ataIsBsy()) {
        inb(ATA_ALT_STATUS_PORT);
        attempts++;
    }

    // Took too long, SET CURR CMD TO NONE?
    if (!ataWaitBsy(false)) {
        kerror(KERN_INFO, "ATA set mult: Disk took too long (status: %x)\n", inb(ATA_STATUS_PORT));
        return _info.currentMultipleSectorSize;
    }

    // Error occurred with the command
    if (ataHasErr())
        return _info.currentMultipleSectorSize;
    
    _info.currentCmd = ATA_NO_CMD;
    _info.currentMultipleSectorSize = numSectors;
    kerror(KERN_DEBUG, "Modified set multiple! (status: %x)\n", inb(ATA_STATUS_PORT));
    return numSectors;
}

/**
 * Reset & initialize the currently selected disk, turns interrupts on
 * FIXME: Fix software reset issue
 * @param ataInfo ATA Information struct
 * @return True if successful, False otherwise
 */
bool ataInitialize() {
    if (!ataWaitBsy(false)) {
        kerror(KERN_ERR, "Disk still busy... (status: %x)\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }
    
    ataToggleInterrupts(false);
    
    if (!ataIdentifyDevice()) {
        kerror(KERN_WARNING, "ATA: Failed to identify!\n");
        return false;
    }

    ataSetMultipleMode(PAGE_SIZE / (2 * _info.sectorSize));

    if (ataGetDrive() < 0) {
        kerror(KERN_WARNING, "ATA: Failed to get Drive #!\n");
        return false;
    }

    while (!ataIsBsy() && ataIsDrq())
        inw(ATA_DATA_PORT);

    kprintf("ATA initialized!\n");
    return true;
}

/**
 * Begin ATA Read transfer with given parameters
 * @param addr Address to place read data into (MUST HAVE sufficient size!)
 * @param startSect Logical sector to begin reading from
 * @param numSectors Number of logical sectors to read
 * @return True if successful, False otherwise
 */
bool ataRead(void* addr, uint32_t startSect, uint16_t numSectors) {
    if (_info.upperMaxNumSectors == 0 && startSect >= _info.lowerMaxNumSectors - numSectors)
        return false;

    _current_addr = (uint16_t*)addr;
    _sectors_remaining = numSectors;
    bool success = true;
    if (_info.hasMultipleCmd) {
        kerror(KERN_DEBUG, "(ata mult read)\n");
        success = ataMultReadSectors(addr, startSect, numSectors, &_info);
    } else {
        success = ataReadSectors(addr, startSect, numSectors, &_info);
    }

    return success;
}

/**
 * Begin ATA Write transfer with given parameters
 * @param addr Address to place write data into (MUST HAVE sufficient size!)
 * @param startSect Logical sector to begin writing from
 * @param numSectors Number of logical sectors to write
 * @return True if successful, False otherwise
 */
bool ataWrite(void* addr, uint32_t startSect, uint16_t numSectors) {
    if (_info.upperMaxNumSectors == 0 && startSect >= _info.lowerMaxNumSectors - numSectors)
        return false;

    _current_addr = (uint16_t*)addr;
    _sectors_remaining = numSectors;
    bool success = true;
    if (_info.hasMultipleCmd) {
        success = ataMultWriteSectors(addr, startSect, numSectors, &_info);
    } else {
        success = ataWriteSectors(addr, startSect, numSectors, &_info);
    }

    return success;
}

/**
 * Interrupt catcher for ATA device: interrupts signaled by ATA devices go through this function
 * @return True if successful, False otherwise
 */
bool ataCmdIrq() {
    kerror(KERN_DEBUG, "ATA IRQ (cmd: %d)!\n", _info.currentCmd);
    bool success = true;
    uint8_t status = ataGetStatus();

    if (status % 2 == 1 || (status & ATA_FAULT_BIT) != 0) {
        kerror(KERN_WARNING, "ATA IRQ Error: (status: %x, error: %x)\n", status, ataGetError());
        success = false;
    } else {
        switch (_info.currentCmd) {
            case ATA_NO_CMD:
                break;
            case ATA_READ_CMD:
                if (_sectors_remaining > 0) {
                    success = ataPioReadTransfer(_current_addr, &_info);
                    if (success) {
                        _sectors_remaining--;
                        _current_addr += _info.sectorSize;
                    }
                }

                break;
            case ATA_WRITE_CMD:
                if (_sectors_remaining > 0) {
                    success = ataPioWriteTransfer(_current_addr, &_info);
                    if (success) {
                        _sectors_remaining--;
                        _current_addr += _info.sectorSize;
                    }
                }
                    
                break;
            case ATA_READ_MULT_CMD:
                if (_sectors_remaining > 0) {
                    success = ataPioMultReadTransfer(_current_addr, &_info);
                    if (success) {
                        _sectors_remaining -= _info.currentMultipleSectorSize;
                        _current_addr += (_info.sectorSize * _info.currentMultipleSectorSize);
                    }
                }

                break;
            case ATA_WRITE_MULT_CMD:
                if (_sectors_remaining > 0) {
                    success = ataPioMultWriteTransfer(_current_addr, &_info);
                    if (success) {
                        _sectors_remaining -= _info.currentMultipleSectorSize;
                        _current_addr += (_info.sectorSize * _info.currentMultipleSectorSize);
                    }
                }

                break;
            case ATA_SOFT_RESET_CMD:
                kerror(KERN_DEBUG, "ATA Soft reset!\n");
                break;
            case ATA_FLUSH_CACHE_CMD:
                // Coming soon...
                break;
            case ATA_IDENT_CMD:
                success = ataPioReadIdentify(&_info);
                break;
        }
    }

    if (success && (status & ATA_BSY_BIT) == 0 && (status & ATA_DRQ_BIT) == 0) {
        _info.currentCmd = ATA_NO_CMD;
    }

    picEoi(14);
    return success;
}
