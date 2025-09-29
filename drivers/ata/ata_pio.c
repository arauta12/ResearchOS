#include <device/ata_pio.h>

/**
 * Do transfer of IDENTIFY DEVICE command (for IRQs and Polling)
 * @param ataInfo ATA Information struct
 * @return True if successful, False if failed
 */
bool ataPioReadIdentify(ata_info_st* ataInfo) {
    // Dev still busy
    if (ataIsBsy()) {
        // Use alt status in case an IRQ comes in the future
        kerror(KERN_INFO, "ATA Ident: disk not ready yet (status: %x)\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }

    // Dev has no data to transfer
    if (!ataIsDrq()) {
        kerror(KERN_INFO, "ATA Ident: no data to transfer (status: %x)\n", inb(ATA_STATUS_PORT));
        ataInfo->currentCmd = ATA_NO_CMD;
        return false;
    }

    uint16_t* identData = (uint16_t*)pmmAllocPages(1);
    for (int i = 0; i < 256; i++)
        identData[i] = inw(ATA_DATA_PORT);

    // Word 0: check if ATA
    if (identData[0] % 2 == 1) {
        kerror(KERN_WARNING, "Disk NOT ATA! (%x)\n", identData[0]);
        pmmFreePages(identData);
        return false;
    }

    // Assert LBA support
    if ((identData[49] & (1 << 9)) == 0) {
        kerror(KERN_WARNING, "ATA: LBA not supported!\n");
        pmmFreePages(identData);
        return false;
    }

    // Assert FLUSH support
    if ((identData[83] & (1 << 13)) == 0) {
        kerror(KERN_WARNING, "ATA: Flush not supported!\n");
        pmmFreePages(identData);
        return false;
    }

    // Word 47: Get max sectors per DRQ mult transfer
    if ((identData[47] & 0xFF) != 0) {
        ataInfo->maxMultipleSectorSize = identData[47] & 0xFF;
        ataInfo->hasMultipleCmd = true;
    }

    // Word 49: Check DMA support
    ataInfo->hasDma = (identData[49] & (1 << 8)) != 0;

    // Word 53: Check word 88 validity
    if ((identData[53] & (1 << 2)) == 0)
        identData[88] = 0;

    // Word 59, Words 100-103: current sectors per DRQ mult transfer
    if ((identData[59] & (1 << 8)) != 0)
        ataInfo->currentMultipleSectorSize = identData[59] & 0xFF;

    ataInfo->lowerMaxNumSectors = (identData[61] << 16) | identData[60];
    ataInfo->upperMaxNumSectors = 0;
    if (identData[61] > 0x0FFF) {
        ataInfo->lowerMaxNumSectors = (identData[101] << 16) | identData[100];
        ataInfo->upperMaxNumSectors = (identData[103] << 16) | identData[102];
    }

    // Word 80: Check latest ATA version supported
    if (identData[80] > 0 && identData[80] < 0xFFFF) {
        int currVersion = 8;
        while (currVersion > 2 && (identData[80] & (1 << currVersion)) == 0)
            currVersion--;
    
        ataInfo->maxAtaVersion = (currVersion > 2)? currVersion : 0; 
    }

    // Word 83: Flush support & 48-bit LBA
    if ((identData[83] & (1 << 10)) != 0)
        ataInfo->hasLba48 = true;
        
    if ((identData[83] & (1 << 12)) != 0)
        ataInfo->hasFlushExt = true;

    // Word 88: udma modes
    if (identData[88] != 0) {
        int selectedUdma = (ataInfo->maxAtaVersion >= 8)? 6 : 5;
        while (selectedUdma >= 0 && (identData[88] & (1 << (selectedUdma + 8))) == 0)
            selectedUdma--;

        ataInfo->currentUdmaMode = selectedUdma;

        int maxUdma = (ataInfo->maxAtaVersion >= 8)? 6 : 5;
        while (maxUdma >= 0 && (identData[88] & (1 << maxUdma)) == 0)
            maxUdma--;

        ataInfo->maxUdmaMode = maxUdma;
    }

    // Words 117-118: Sector size in words ONLY for ATA8
    ataInfo->sectorSize = 256;
    if (ataInfo->maxAtaVersion >= 8)
        ataInfo->sectorSize = (identData[118] << 16) | identData[117];

#ifdef DEBUG
    kprintf("ATA ident: dma? %d (%x)\n", ataInfo->hasDma, identData[49]);
    kprintf("ATA ident: max DRQ block sectors = %x (%x)\n", ataInfo->maxMultipleSectorSize, identData[47]);
    kprintf("ATA ident: curr DRQ block sectors = %x (%x)\n", ataInfo->currentMultipleSectorSize, identData[59]);
    kprintf("ATA ident: LBA sectors = %x %x\n", ataInfo->upperMaxNumSectors, ataInfo->lowerMaxNumSectors);
    kprintf("ATA ident: FLUSH ext supported = %d (%x)\n", ataInfo->hasFlushExt, identData[83]);
    kprintf("ATA ident: 48-bit LBA supported = %d (%x)\n", ataInfo->hasLba48, identData[83]);
    kprintf("ATA ident: sector size = %x words\n", ataInfo->sectorSize);
    kprintf("ATA ident: mult? = %d (%x)\n", ataInfo->hasMultipleCmd);
    kprintf("ATA ident: max version = %d\n", ataInfo->maxAtaVersion);
#endif

    return pmmFreePages(identData);
}

/**
 * Do transfer of READ SECTOR(S) (ext) command (for IRQs and Polling).
 * 
 * NOTE: Assumes buffer is sufficient in size
 * @param addr Address to transfer data to
 * @param ataInfo ATA Information struct
 * @return True if successful, False if failed
 */
bool ataPioReadTransfer(void* addr, ata_info_st* ataInfo) {
    // Dev still busy
    if (ataIsBsy()) {
        // Use alt status in case an IRQ comes in the future
        kerror(KERN_INFO, "ATA PIO READ: disk not ready yet (status: %x)\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }

    // Dev has no data to transfer
    if (!ataIsDrq()) {
        // Read the status to clear up a potential IRQ for this
        kerror(KERN_INFO, "ATA PIO READ: no data to transfer (status: %x)\n", inb(ATA_STATUS_PORT));
        ataInfo->currentCmd = ATA_NO_CMD;
        return false;
    }

    uint16_t* transferAddr = (uint16_t*)addr;
    for (uint32_t i = 0; i < ataInfo->sectorSize; i++)
        transferAddr[i] = inw(ATA_DATA_PORT);

#ifdef DEBUG
    kerror(KERN_DEBUG, "ATA PIO READ:\n");
    for (int i = 0; i < 8; i++)
        kprintf("\t%x ", transferAddr[i]);
        
    kprintf("\n");
#endif

    return true;
}

/**
 * Do transfer of READ MULTIPLE (ext) command (for IRQs and Polling).
 * 
 * NOTE: Assumes buffer is sufficient in size
 * @param addr Address to transfer data to
 * @param ataInfo ATA Information struct
 * @return True if successful, False if failed
 */
bool ataPioMultReadTransfer(void* addr, ata_info_st* ataInfo) {
    // Dev still busy
    if (ataIsBsy()) {
        // Use alt status in case an IRQ comes in the future
        kerror(KERN_INFO, "ATA PIO MULT READ: disk not ready yet (status: %x)\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }

    // Dev has no data to transfer
    if (!ataIsDrq()) {
        // Read the status to clear up a potential IRQ for this
        kerror(KERN_INFO, "ATA PIO MULT READ: no data to transfer (status: %x)\n", inb(ATA_STATUS_PORT));
        ataInfo->currentCmd = ATA_NO_CMD;
        return false;
    }

    uint16_t* transferAddr = (uint16_t*)addr;
    for (uint32_t i = 0; i < ataInfo->currentMultipleSectorSize * ataInfo->sectorSize; i++)
        transferAddr[i] = inw(ATA_DATA_PORT);

    // Debug
#ifdef DEBUG
    kerror(KERN_DEBUG, "ATA PIO MULT READ data:\n");
    for (int s = 0; s < ataInfo->currentMultipleSectorSize; s++) {
        for (int i = 0; i < 8; i++)
        kprintf("\t%x ", transferAddr[i + ataInfo->sectorSize * s]);
        kprintf("\n");
    }
    #endif    
    
    kerror(KERN_DEBUG, "ATA PIO MULT READ finished:\n");
    return true;
}

/**
 * Do transfer of WRITE SECTOR(S) command (for IRQs and Polling).
 * 
 * NOTE: Assumes buffer is sufficient in size
 * @param addr Address to transfer data from
 * @param ataInfo ATA Information struct
 * @return True if successful, False if failed
 */
bool ataPioWriteTransfer(void* addr, ata_info_st* ataInfo) {
    // Dev still busy
    if (ataIsBsy()) {
        // Use alt status in case an IRQ comes in the future
        kerror(KERN_INFO, "ATA PIO WRITE: disk not ready yet (status: %x)\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }

    // Dev has no data to transfer
    if (!ataIsDrq()) {
        // Read the status to clear up a potential IRQ for this
        kerror(KERN_INFO, "ATA PIO WRITE: no data to transfer (status: %x)\n", inb(ATA_STATUS_PORT));
        ataInfo->currentCmd = ATA_NO_CMD;
        return false;
    }

    uint16_t* transferAddr = (uint16_t*)addr;
    for (uint32_t i = 0; i < ataInfo->sectorSize; i++)
        outw(ATA_DATA_PORT, transferAddr[i]);

    // Debug
#ifdef DEBUG
    kerror(KERN_DEBUG, "ATA PIO WRITE:\n");
    for (int i = 0; i < 8; i++)
        kprintf("\t%x ", transferAddr[i]);
        
    kprintf("\n");
#endif

    return true;
}

/**
 * Do transfer of WRITE MULTIPLE (ext) command (for IRQs and Polling).
 * 
 * NOTE: Assumes buffer is sufficient in size
 * @param addr Address to transfer data from
 * @param ataInfo ATA Information struct
 * @return True if successful, False if failed
 */
bool ataPioMultWriteTransfer(void* addr, ata_info_st* ataInfo) {
    // Dev still busy
    if (ataIsBsy()) {
        // Use alt status in case an IRQ comes in the future
        kerror(KERN_INFO, "ATA PIO MULT WRITE: disk not ready yet (status: %x)\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }

    // Dev has no data to transfer
    if (!ataIsDrq()) {
        // Read the status to clear up a potential IRQ for this
        kerror(KERN_INFO, "ATA PIO MULT WRITE: no data to transfer (status: %x)\n", inb(ATA_STATUS_PORT));
        ataInfo->currentCmd = ATA_NO_CMD;
        return false;
    }

    uint16_t* transferAddr = (uint16_t*)addr;
    for (uint32_t i = 0; i < ataInfo->currentMultipleSectorSize * ataInfo->sectorSize; i++)
        outw(ATA_DATA_PORT, transferAddr[i]);

#ifdef DEBUG
    kerror(KERN_DEBUG, "ATA PIO MULT WRITE:\n");
    for (int s = 0; s < ataInfo->currentMultipleSectorSize; s++) {
        for (int i = 0; i < 8; i++)
            kprintf("%x ", transferAddr[i + ataInfo->sectorSize * s]);
        kprintf("\n");
    }
#endif

    return true;
}

/**
 * Send a READ SECTORS(S) (ext) command to the ATA disk with given parameters. If interrupts are
 * disabled, also performs the transfer. Modifies current cmd if certain cmd started / ended.
 * @param addr Transfer address if IRQ is disabled
 * @param startSect Starting sector LBA address (can be 28 or 48-bit (if supported) LBA)
 * @param numSectors Number of sectors to transfer starting from startSect
 * @param ataInfo ATA Information struct
 * @return True if successful, False if something went wrong
 */
bool ataReadSectors(void* addr, uint32_t startSect, uint16_t numSectors, ata_info_st* ataInfo) {
    if (!ataWaitBsy(false) || !ataWaitDrq(false) || !ataWaitDrdy(true)) {
        kerror(KERN_INFO, "ATA Read cmd: Dev still busy (status: %x)\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }

    // This portion of the command depends on whether LBA 48 is supported
    if (ataInfo->hasLba48) {
        outb(ATA_SECT_COUNT_PORT, (numSectors >> 8) & 0xFF);
        outb(ATA_LBA_LOW_PORT, (startSect >> 24) & 0xFF);
        outb(ATA_LBA_MID_PORT, 0);
        outb(ATA_LBA_HI_PORT, 0);
    } else {
        uint8_t devInfo = inb(ATA_DEVICE_PORT);
        outb(ATA_DEVICE_PORT, devInfo | ((startSect >> 24) & 0xF));
    }

    outb(ATA_SECT_COUNT_PORT, numSectors & 0xFF);
    outb(ATA_LBA_LOW_PORT, startSect & 0xFF);
    outb(ATA_LBA_MID_PORT, (startSect >> 8) & 0xFF);
    outb(ATA_LBA_HI_PORT, (startSect >> 16) & 0xFF);

    // Begin command
    ataInfo->currentCmd = ATA_READ_CMD;
    outb(ATA_CMD_PORT, (ataInfo->hasLba48)? ATA_READ_SECTS_EXT : ATA_READ_SECTS);

    // Wait a bit so we can check the status for an error
    ataWait400ns();
    
    // Now let's check the status
    if (!ataIsBsy() && ataHasErr()) {
        kerror(KERN_INFO, "ATA Read cmd: Error (status: %x, code: %x)!\n", inb(ATA_STATUS_PORT), inb(ATA_ERROR_PORT));
        ataInfo->currentCmd = ATA_NO_CMD;
        return false;
    }

    // Skip transfer if IRQ is enabled
    if (ataInfo->intEnabled)
        return true;

    if (addr == NULL)
        return false;

    // Wait for transfer... (assuming an IO read is 30ns)
    const int maxAttempts = 100000;
    int attempts = 0;
    while (attempts < maxAttempts && ataIsBsy())
        inb(ATA_ALT_STATUS_PORT);

    if (!ataWaitBsy(false) || !ataWaitDrq(true)) {
        kerror(KERN_INFO, "ATA Read cmd: Took too long (status: %x)!\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }

    uint16_t sectRemaining = numSectors;
    while (sectRemaining > 0) {
        if (!ataPioReadTransfer(addr, ataInfo))
            return false;

        sectRemaining--;
    }
    
    ataInfo->currentCmd = ATA_NO_CMD;
    return true;
}

/**
 * Send a WRITE SECTORS(S) (ext) command to the ATA disk with given parameters. If interrupts are
 * disabled, also performs the transfer.
 * @param addr Transfer address
 * @param startSect Starting sector LBA address (can be 28 or 48-bit (if supported) LBA)
 * @param numSectors Number of sectors to transfer starting from startSect
 * @param ataInfo ATA Information struct
 * @return True if successful, False if something went wrong
 */
bool ataWriteSectors(void* addr, uint32_t startSect, uint16_t numSectors, ata_info_st* ataInfo) {
    if (!ataWaitBsy(false) || !ataWaitDrq(false) || !ataWaitDrdy(true)) {
        kerror(KERN_INFO, "ATA Read cmd: Dev still busy (status: %x)\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }

    // This portion of the command depends on whether LBA 48 is supported
    if (ataInfo->hasLba48) {
        outb(ATA_SECT_COUNT_PORT, (numSectors >> 8) & 0xFF);
        outb(ATA_LBA_LOW_PORT, (startSect >> 24) & 0xFF);
        outb(ATA_LBA_MID_PORT, 0);
        outb(ATA_LBA_HI_PORT, 0);
    } else {
        uint8_t devInfo = inb(ATA_DEVICE_PORT);
        outb(ATA_DEVICE_PORT, devInfo | ((startSect >> 24) & 0xF));
    }

    outb(ATA_SECT_COUNT_PORT, numSectors & 0xFF);
    outb(ATA_LBA_LOW_PORT, startSect & 0xFF);
    outb(ATA_LBA_MID_PORT, (startSect >> 8) & 0xFF);
    outb(ATA_LBA_HI_PORT, (startSect >> 16) & 0xFF);

    // Begin command
    ataInfo->currentCmd = ATA_WRITE_CMD;
    outb(ATA_CMD_PORT, (ataInfo->hasLba48)? ATA_WRITE_SECTS_EXT : ATA_WRITE_SECTS);

    // Wait a bit so we can check the status for an error
    ataWait400ns();
    
    // Now let's check the status
    if (!ataIsBsy() && ataHasErr()) {
        kerror(KERN_INFO, "ATA Read cmd: Error (code: %x)!\n", inb(ATA_ERROR_PORT));
        ataInfo->currentCmd = ATA_NO_CMD;
        return false;
    }

    if (addr == NULL)
        return false;

    // Wait for transfer... (assuming an IO read is 30ns)
    const int maxAttempts = 100000;
    int attempts = 0;
    while (attempts < maxAttempts && ataIsBsy())
        inb(ATA_ALT_STATUS_PORT);

    if (!ataWaitBsy(false) || !ataWaitDrq(true)) {
        kerror(KERN_INFO, "ATA Read cmd: Took too long (status: %x)!\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }

    uint16_t sectRemaining = numSectors;
    do {
        if (!ataPioMultWriteTransfer(addr, ataInfo))
            return false;

        sectRemaining--;
        if (ataInfo->intEnabled)
            return true;    
    } while (sectRemaining > 0);
    
    ataInfo->currentCmd = ATA_NO_CMD;
    return true;
}

/**
 * Send a READ MULTIPLE (ext) command to the ATA disk with given parameters. If interrupts are
 * disabled, also performs the transfer.
 * @param addr Transfer address
 * @param startSect Starting sector LBA address (can be 28 or 48-bit (if supported) LBA)
 * @param numSectors Number of sectors to transfer starting from startSect
 * @param ataInfo ATA Information struct
 * @return True if successful, False if something went wrong
 */
bool ataMultReadSectors(void* addr, uint32_t startSect, uint16_t numSectors, ata_info_st* ataInfo) {
    if (!ataWaitBsy(false) || !ataWaitDrq(false) || !ataWaitDrdy(true)) {
        kerror(KERN_INFO, "ATA Read mult cmd: Dev still busy (status: %x)\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }

    // This portion of the command depends on whether LBA 48 is supported
    if (ataInfo->hasLba48) {
        outb(ATA_SECT_COUNT_PORT, (numSectors >> 8) & 0xFF);
        outb(ATA_LBA_LOW_PORT, (startSect >> 24) & 0xFF);
        outb(ATA_LBA_MID_PORT, 0);
        outb(ATA_LBA_HI_PORT, 0);
    } else {
        uint8_t devInfo = inb(ATA_DEVICE_PORT);
        outb(ATA_DEVICE_PORT, devInfo | ((startSect >> 24) & 0xF));
    }

    outb(ATA_SECT_COUNT_PORT, numSectors & 0xFF);
    outb(ATA_LBA_LOW_PORT, startSect & 0xFF);
    outb(ATA_LBA_MID_PORT, (startSect >> 8) & 0xFF);
    outb(ATA_LBA_HI_PORT, (startSect >> 16) & 0xFF);

    // Begin command
    ataInfo->currentCmd = ATA_READ_MULT_CMD;
    outb(ATA_CMD_PORT, (ataInfo->hasLba48)? ATA_READ_MULT_EXT : ATA_READ_MULT);

    // Wait a bit so we can check the status for an error
    ataWait400ns();
    
    // Now let's check the status
    if (!ataIsBsy() && ataHasErr()) {
        kerror(KERN_INFO, "ATA Read mult cmd: Error (code: %x)!\n", inb(ATA_ERROR_PORT));
        ataInfo->currentCmd = ATA_NO_CMD;
        return false;
    }

    // Skip transfer if IRQ is enabled
    if (ataInfo->intEnabled)
        return true;

    if (addr == NULL) {
        kerror(KERN_INFO, "ATA Mult read addr is NULL!\n");
        return false;
    }

    // Wait for transfer... (assuming an IO read is 30ns)
    const int maxAttempts = 100000;
    int attempts = 0;
    while (attempts < maxAttempts && ataIsBsy())
        inb(ATA_ALT_STATUS_PORT);

    if (!ataWaitBsy(false) || !ataWaitDrq(true)) {
        kerror(KERN_INFO, "ATA Read mult cmd: Took too long (status: %x)!\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }
    
    uint16_t sectRemaining = numSectors;
    while (sectRemaining > 0) {
        if (!ataPioMultReadTransfer(addr, ataInfo)) {
            kerror(KERN_INFO, "ATA PIO MULT read failed!\n");
            return false;
        }

        sectRemaining -= ataInfo->currentMultipleSectorSize;
    }
    
    ataInfo->currentCmd = ATA_NO_CMD;

    kerror(KERN_DEBUG, "ATA Mult read finished\n");
    return true;
}

/**
 * Send a WRITE MULTIPLE (ext) command to the ATA disk with given parameters. If interrupts are
 * disabled, also performs the transfer.
 * @param addr Transfer address
 * @param startSect Starting sector LBA address (can be 28 or 48-bit (if supported) LBA)
 * @param numSectors Number of sectors to transfer starting from startSect
 * @param ataInfo ATA Information struct
 * @return True if successful, False if something went wrong
 */
bool ataMultWriteSectors(void* addr, uint32_t startSect, uint16_t numSectors, ata_info_st* ataInfo) {
    if (!ataWaitBsy(false) || !ataWaitDrq(false) || !ataWaitDrdy(true)) {
        kerror(KERN_INFO, "ATA Write mult cmd: Dev still busy (status: %x)\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }

    // This portion of the command depends on whether LBA 48 is supported
    if (ataInfo->hasLba48) {
        outb(ATA_SECT_COUNT_PORT, (numSectors >> 8) & 0xFF);
        outb(ATA_LBA_LOW_PORT, (startSect >> 24) & 0xFF);
        outb(ATA_LBA_MID_PORT, 0);
        outb(ATA_LBA_HI_PORT, 0);
    } else {
        uint8_t devInfo = inb(ATA_DEVICE_PORT);
        outb(ATA_DEVICE_PORT, devInfo | ((startSect >> 24) & 0xF));
    }

    outb(ATA_SECT_COUNT_PORT, numSectors & 0xFF);
    outb(ATA_LBA_LOW_PORT, startSect & 0xFF);
    outb(ATA_LBA_MID_PORT, (startSect >> 8) & 0xFF);
    outb(ATA_LBA_HI_PORT, (startSect >> 16) & 0xFF);

    // Begin command
    ataInfo->currentCmd = ATA_WRITE_MULT_CMD;
    outb(ATA_CMD_PORT, (ataInfo->hasLba48)? ATA_WRITE_MULT_EXT : ATA_WRITE_MULT);

    // Wait a bit so we can check the status for an error
    ataWait400ns();
    
    // Now let's check the status
    if (!ataIsBsy() && ataHasErr()) {
        kerror(KERN_INFO, "ATA Write mult cmd: Error (code: %x)!\n", inb(ATA_ERROR_PORT));
        ataInfo->currentCmd = ATA_NO_CMD;
        return false;
    }

    if (addr == NULL)
        return false;

    // Wait for transfer... (assuming an IO read is 30ns)
    const int maxAttempts = 100000;
    int attempts = 0;
    while (attempts < maxAttempts && ataIsBsy())
        inb(ATA_ALT_STATUS_PORT);

    if (!ataWaitBsy(false) || !ataWaitDrq(true)) {
        kerror(KERN_INFO, "ATA Write mult cmd: Took too long (status: %x)!\n", inb(ATA_ALT_STATUS_PORT));
        return false;
    }

    uint16_t sectRemaining = numSectors;
    do {
        if (!ataPioMultWriteTransfer(addr, ataInfo))
            return false;

        sectRemaining -= ataInfo->currentMultipleSectorSize;
        if (ataInfo->intEnabled)
            return true;    
    } while (sectRemaining > 0);
    
    ataInfo->currentCmd = ATA_NO_CMD;
    return true;
}
