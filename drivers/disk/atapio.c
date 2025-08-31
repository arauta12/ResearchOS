#include <device/atapio.h>

static bool _flush_supported = false;
static bool _lba_supported = false;
static bool _flush_supported_ext = false;
static bool _sector_multiple_supported = false;

static ATA_ADDR_MODE _curr_mode = NO_MODE;
static int _drive_selected = -1;
static bool _int_enabled = false;
static uint32_t _num_usable_sectors = 0;
static int _max_attempts = 3000;
static uint16_t _max_drq_block_sectors = 1;
static uint16_t _drq_block_sectors = 1;

static uint8_t _last_cmd = ATA_NO_CMD;
static uint8_t _num_sectors_transferred = 0;
static uint8_t _num_sectors_remaining = 0;
static uint16_t* _curr_transfer_addr = NULL;

/**
 * Waits for a maximum number of iterations for the BSY bit
 * to be equal to given value
 * @param bsy True for BSY=1, False for BSY=0
 * @return True if successful, False otherwise
 */
static bool _wait_bsy(bool bsy) {
    int attempts = 0;

    uint8_t statusMask = inb(ATA_ALT_STATUS_PORT) & (1 << 7);
    while (attempts < _max_attempts) {
        if ((bsy && statusMask != 0) || (!bsy && statusMask == 0))
            return true;

        io_wait();
        statusMask = inb(ATA_ALT_STATUS_PORT) & (1 << 7);
        attempts++;
    }

    return false;
}

/**
 * Waits for a maximum number of iterations for the DRDY bit
 * to be equal to given value
 * @param drdy True for DRDY=1, False for DRDY=0
 * @return True if successful, False otherwise
 */
static bool _wait_drdy(bool drdy) {
    int attempts = 0;

    uint8_t statusMask = inb(ATA_ALT_STATUS_PORT) & (1 << 6);
    while (attempts < _max_attempts) {
        if ((drdy && statusMask != 0) || (!drdy && statusMask == 0))
            return true;

        io_wait();
        statusMask = inb(ATA_ALT_STATUS_PORT) & (1 << 6);
        attempts++;
    }

    return false;
}

/**
 * Waits for a maximum number of iterations for the DRQ bit
 * to be equal to given value
 * @param drq True for DRQ=1, False for DRQ=0
 * @return True if successful, False otherwise
 */
static bool _wait_drq(bool drq) {
    int attempts = 0;

    uint8_t statusMask = inb(ATA_ALT_STATUS_PORT) & (1 << 3);
    while (attempts < _max_attempts) {
        if ((drq && statusMask != 0) || (!drq && statusMask == 0))
            return true;

        io_wait();
        statusMask = inb(ATA_ALT_STATUS_PORT) & (1 << 3);
        attempts++;
    }

    return false;
}

/**
 * Checks if DRQ bit is set in a Status register (based on parameter).
 * @return True if DRQ bit is set, False if cleared 
 */
static bool _is_drq() {
    uint8_t statusMask = inb(ATA_ALT_STATUS_PORT) & (1 << 3);
    return (statusMask != 0);
}

/**
 * Checks if BSY bit is set in a Status register (based on parameter).
 * @return True if BSY bit is set, False if cleared 
 */
static bool _is_bsy() {
    uint8_t statusMask = inb(ATA_ALT_STATUS_PORT) & (1 << 7);
    return (statusMask != 0);
}

/**
 * Checks if ERR bit is set in a Status register (based on parameter).
 * @return True if ERR bit is set, False if cleared 
 */
static bool _has_err() {
    return (ATA_ALT_STATUS_PORT % 2 == 1);
}

/**
 * Waits at least 400ns to ensure drive has enough time to update the status after a register write
 * @return None
 */
static void _wait_400ns() {
    for (int i = 0; i < 20; i++)
        inb(ATA_ALT_STATUS_PORT);
}

/**
 * Enable/disable interrupts for the current drive
 * NOTE: Assumes toggle was successful (ie. no error)
 * @param enable True to enable, False to disable
 * @return None
 */
void ataToggleInterrupts(bool enable) {
    uint8_t ctrl = inb(ATA_DEV_CTRL_PORT);
    io_wait();
    
    outb(ATA_DEV_CTRL_PORT, (ctrl & 0xFD) | ((!enable) << 1));
    _int_enabled = enable;

    _wait_400ns();
}

/**
 * Gives currently selected drive (0 for Device 0, 1 for Device 1) on current channel
 * @return 0 for Device 0 or 1 for Device 1, -1 for an error
 */
int ataGetDrive() {
    if (!_wait_bsy(false)) {
        kerror(KERN_INFO, "ATA get drive: drive still busy\n");
        return -1;
    }

    uint8_t devData = inb(ATA_DEVICE_PORT);
    return ((devData & (1 << 4)) != 0);
}

/**
 * Change current drive for current channel: False for Device 0, True for Device 1
 * @param dev1 False for Device 0, True for Device 1
 * @return True if changed successfully, False if an issue occurred
 */
bool ataSetDrive(bool dev1) {
    if (!_wait_bsy(false) || !_wait_drq(false)) {
        kerror(KERN_INFO, "ATA set drive: drive still busy\n");
        return false;
    }

    uint8_t devData = inb(ATA_DEVICE_PORT);
    outb(ATA_DEVICE_PORT, (devData & 0xEF) | (dev1 << 4));
    _wait_400ns();

    const int MAX_ATTEMPTS = 5;
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS && _is_bsy()) {
        _wait_400ns();
        attempts++;
    }

    if (_is_bsy()) {
        kerror(KERN_WARNING, "ATA Set drive: drive still busy\n");
        return false;
    }

    if (_has_err()) {
        kerror(KERN_WARNING, "ATA Set drive: Error! (status: %x, error: %x)\n", inb(ATA_STATUS_PORT), inb(ATA_ERROR_PORT));
        return false;
    }

    return true;
}

/**
 * Do Identify device command for Non IRQ mode
 * @return True if success, False otherwise
 */
bool ataIdentifyDevice() {
    if (!_wait_bsy(false) || !_wait_drq(false) || !_wait_drdy(true)) {
        uint8_t status = inb(ATA_STATUS_PORT);
        kerror(KERN_INFO, "ATA Identify: drive still not ready (status %x)\n", status);
        return false;
    }

    outb(ATA_CMD_PORT, ATA_IDENTIFY_DEV);
    _last_cmd = ATA_IDENTIFY_DEV;
    _wait_400ns();

    if (_has_err()) {
        uint8_t status = inb(ATA_STATUS_PORT);
        kerror(KERN_INFO, "ATA Identify: drive still not ready / has error (status %x)\n", status);
        _last_cmd = ATA_NO_CMD;
        return false;
    }

    bool isAta = true;
    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(ATA_DATA_PORT);

        if (!isAta)
            continue;

        switch (i) {
            case 0:
                isAta = (data & (1 << 15)) == 0;
                break;
            case 47:
                uint8_t maxNum = data & 0xFF;
                if (maxNum != 0)
                    _max_drq_block_sectors = maxNum;
            case 49:
                _lba_supported = (data & (1 << 8)) != 0;
                break;
            case 59:
                if ((data & (1 << 8)) != 0) {
                    _sector_multiple_supported = true;
                    _drq_block_sectors = data & 0xFF;
                }
            case 60:
                _num_usable_sectors = ((inw(ATA_DATA_PORT) << 16) | data) - 1;
                i++;
                break;
            case 80:
                if (data != 0 && data != 0xFFFF)
                    kerror(KERN_DEBUG, "ATA IDENT: supported versions = %x\n", data);

                break;
            case 83:
                _flush_supported = (data & (1 << 12)) != 0;
                _flush_supported_ext = (data & (1 << 13)) != 0;
                break;

            io_wait();
        }
    }

    if (!isAta) {
        kerror(KERN_DEBUG, "Disk not ATA!\n");
        return false;
    }

    _last_cmd = ATA_NO_CMD;
}

/**
 * Do software reset
 * @return True if successful, False if failed
 */
bool ataSoftwareReset() {
    if (!_wait_bsy(false)) {
        kerror(KERN_WARNING, "ATA Soft Reset: Drive still busy!\n");
        return false;
    }

    uint8_t devCtrl = inb(ATA_DEV_CTRL_PORT);
    if ((devCtrl & ATA_SRST_BIT) != 0) {
        kerror(KERN_INFO, "ATA Soft Reset: reset already in progress\n");
        return false;
    }

    outb(ATA_DEV_CTRL_PORT, devCtrl | ATA_SRST_BIT);

    // TODO: Replace with timer equivalent
    // Wait min 5 microseconds
    for (int i = 0; i < 400; i++)
        io_wait();

    outb(ATA_DEV_CTRL_PORT, devCtrl & (~ATA_SRST_BIT));

    // Wait minimum 2ms
    const int MAX_ATTEMPTS = 2;
    int attempts = 0;

    while (attempts < MAX_ATTEMPTS && _is_bsy()) {
        for (int i = 0; i < 140000; i++)
            io_wait();
        
        attempts++;
    }

    if (_is_bsy()) {
        kerror(KERN_WARNING, "ATA Soft Reset: Timed out!\n");
        return false;
    }

    ataToggleInterrupts(_int_enabled);

    uint8_t status = inb(ATA_STATUS_PORT);
    if ((status % 2 == 1) || ((status & ATA_FAULT_BIT) != 0)) {
        kerror(KERN_WARNING, "ATA Soft Reset: Error! (status: %x, error: %x)\n", status, inb(ATA_ERROR_PORT));
        return false;
    }

    return true;
}

/**
 * TODO:
    Disable INTs for BOTH devices
    Do software reset
    Do identify
    Init state variables
    Flush buffer (if DRQ = 1)
    Clear PIC mask bit for IRQ 14
    Enable INTs for drive of interest
 */
bool ataInitialize(int diskNum) {
    if (!_wait_bsy(false)) {
        kerror(KERN_ERR, "Disk still busy... (status: %x)\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }
    
    ataToggleInterrupts(false);
    
    ataSoftwareReset();
    kerror(KERN_DEBUG, "ATA Reset complete!\n");

    ataIdentifyDevice();

    if (!_lba_supported) {
        kerror(KERN_WARNING, "LBA is not supported!\n");
        return false;
    }

    _curr_mode = LBA28;
    _drive_selected = ataGetDrive();

    if (_drive_selected < 0) {
        kerror(KERN_ERR, "Failed to verify drive!\n");
        return false;
    }

    if (diskNum != _drive_selected) {
        kerror(KERN_ERR, "This disk not supported yet (%d, selected: %d)\n", diskNum, _drive_selected);
        return false;
    }

    while (_is_drq())
        inw(ATA_DATA_PORT);

    clearMaskIrq(14);
    ataToggleInterrupts(true);

    kprintf("ATA initialized!\n");
}

/**
 * TODO: Update & separate into distinct ones
 */
bool ataSectorsRW(void* addr, uint32_t startSect, uint8_t numSectors, bool readMode) {
    // Check if we the right environment to do this (CHS not supported yet!)
    if (!_lba_supported || _last_cmd != ATA_NO_CMD || _drive_selected < 0 || !_int_enabled) {
        kerror(KERN_DEBUG, "ATA rw sector: Invalid settings!\n");
        return false;
    }

    // Since we are using the current drive, just wait until it is ready for a new command
    if (!_wait_bsy(false) || !_wait_drq(false)) {
        kerror(KERN_INFO, "ATA rw sector cmd: disk still busy (%x)\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }

    // We are set, so set the parameters & send the command
    uint8_t devData = inb(ATA_DEVICE_PORT) & 0xB0;
    
    outb(ATA_SECT_COUNT_PORT, numSectors);
    outb(ATA_LBA_LOW_PORT, (uint8_t)(startSect & 0xFF));
    outb(ATA_LBA_MID_PORT, (uint8_t)((startSect >> 8) & 0xFF));
    outb(ATA_LBA_HI_PORT, (uint8_t)((startSect >> 16) & 0xFF));
    outb(ATA_DEVICE_PORT, (devData | (1 << 6) | ((startSect >> 24) & 0x0F)));

    _curr_transfer_addr = (uint16_t*)addr;
    _num_sectors_remaining = numSectors;
    _num_sectors_transferred = 0;

    if (readMode) {
        _last_cmd = ATA_READ_SECTS;
        outb(ATA_CMD_PORT, ATA_READ_SECTS);
    } else {
        _last_cmd = ATA_WRITE_SECTS;
        outb(ATA_CMD_PORT, ATA_WRITE_SECTS);
    }

    // Verify that the command was successful
    // NOTE: ATA recommends we wait at least 400ns after sending a command to check status
    for (int i = 0; i < 20; i++)
        inb(ATA_ALT_STATUS_PORT);
    
    if (!_is_bsy(true) && _has_err(true)) {
        kerror(KERN_INFO, "ATA Read failed! Status: %x, Error: %x\n", inb(ATA_ALT_STATUS_PORT), inb(ATA_ERROR_PORT));
        _curr_transfer_addr = NULL;
        _num_sectors_remaining = 0;
        _num_sectors_transferred = 0;
        _last_cmd = ATA_NO_CMD;
        return false;
    }

    if (_last_cmd == ATA_WRITE_SECTS && !_is_bsy(true) && _is_drq(true)) {
        for (int i = 0; i < 256; i++) {
            uint16_t data = _curr_transfer_addr[i];
            outw(ATA_DATA_PORT, data); 
        }
    }

    return true;
}

/**
 * TODO: Move this to drive_main
 */
void ataSectorIrq() {
    uint8_t status = inb(ATA_STATUS_PORT);

    if (status % 2 == 1) {
        kerror(KERN_ERR, "ATA IRQ ERROR: %x\n", inb(ATA_ERROR_PORT));
    } else if (_last_cmd == ATA_NO_CMD || _curr_transfer_addr == NULL || _num_sectors_remaining == 0) {
        kerror(KERN_INFO, "ATA IRQ: No cmd detected for this!\n");
    } else {
        /*
            Disk sent an IRQ still being busy? Shouldn't happen but...
            Not exactly sure what to do, 
                lets end this interrupt early (perhaps will assert another when ready)
        */
        if ((status & (1 << 7)) != 0) {
            kerror(KERN_ERR, "ATA IRQ: Disk still busy?\n");
            picEoi(14);
            return;
        }

        // Disk cmd is completed w/ no error
        if ((status & (1 << 3)) == 0) {
            kerror(KERN_DEBUG, "ATA IRQ: status = %x!\n", status);
            if (status % 2 == 0) {
                kerror(KERN_DEBUG, "ATA IRQ: Cmd complete!\n");
            } else {
                kerror(KERN_ERR, "ATA IRQ: ERROR (%x)!\n", inb(ATA_ERROR_PORT));
            }

            _last_cmd = ATA_NO_CMD;
            _curr_transfer_addr = NULL;
            _num_sectors_remaining = 0;
            _num_sectors_transferred = 0;
            
            picEoi(14);
            return;
        }

        switch (_last_cmd) {
            case ATA_READ_SECTS:
                for (int i = 0; i < 256; i++)
                    _curr_transfer_addr[256 * _num_sectors_transferred + i] = inw(ATA_DATA_PORT);

                _num_sectors_remaining--;
                _num_sectors_transferred++;
                break;
            case ATA_WRITE_SECTS:
                _num_sectors_remaining--;
                _num_sectors_transferred++;

                kerror(KERN_DEBUG, "ATA IRQ!: remaining = %x, transferred = %x\n", _num_sectors_remaining, _num_sectors_transferred);

                if (_num_sectors_remaining == 0)
                    break;

                for (int i = 0; i < 256; i++) {
                    uint16_t data = _curr_transfer_addr[256 * _num_sectors_transferred + i];
                    outw(ATA_DATA_PORT, data); 
                }
                break;
            default:
                kerror(KERN_INFO, "ATA IRQ: Unknown command for IRQ\n");
        }

        // Debugging
        if (_num_sectors_remaining == 0) {
            kerror(KERN_DEBUG, "ATA transferred %d sectors\n", _num_sectors_transferred);
            if (_last_cmd == ATA_READ_SECTS) {
                uint8_t* addr = (uint8_t*)_curr_transfer_addr;
            
                // Print first 16 bytes of each sector to verify
                for (int n = 0; n < _num_sectors_transferred; n++) {
                    for (int i = 0; i < 8; i++)
                        kprintf("%x ", addr[i + n * 512]);
                    kprintf("\n(END SECT #%d)\n", n + 1);
                }
            }

            _last_cmd = ATA_NO_CMD;
            _curr_transfer_addr = NULL;
        }
    }

    picEoi(14);
}
