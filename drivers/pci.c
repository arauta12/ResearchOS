#include <device/pci.h>
#include <io.h>

/**
 * Reads double word in PCI Configuration Space of the specified device.
 * NOTE: regOffset must be a multiple of 4 for this function, otherwise 
 * it will be converted down to the nearest dword multiple (ie. 5 -> 4)
 * @param dev Pointer to PCI device struct (must have valid bus, dev & fn)
 * @param regOffset The byte offset from the Config space of the device
 * @return The double word data from that offset
 */
static uint32_t _pci_read_config_dword(pci_dev_st* dev, uint8_t regOffset) {
    outl(PCI_CONFIG_ADDR_PORT, PCI_CONFIG_ADDR_BIT | PCI_ADDR(dev->busNum, dev->devNum, dev->functNum, regOffset));

    io_wait();
    
    return (inl(PCI_CONFIG_DATA_PORT) >> ((regOffset % 4) * 8));
}

/**
 * Reads a word in PCI Configuration Space of the specified device.
 * NOTE: regOffset must be a multiple of 2 for this function, otherwise 
 * it will be converted down to the nearest word multiple (ie. 7 -> 6)
 * @param dev Pointer to PCI device struct (must have valid bus, dev & fn)
 * @param regOffset The byte offset from the Config space of the device
 * @return The word data from that offset
 */
static uint16_t _pci_read_config_word(pci_dev_st* dev, uint8_t regOffset) {
    outl(PCI_CONFIG_ADDR_PORT, PCI_CONFIG_ADDR_BIT | PCI_ADDR(dev->busNum, dev->devNum, dev->functNum, regOffset));

    io_wait();

    uint32_t resp = inl(PCI_CONFIG_DATA_PORT);
    return (uint16_t)(resp >> ((regOffset & 0x2) * 8));
}

/**
 * Reads a byte in PCI Configuration Space of the specified device.
 * @param dev Pointer to PCI device struct (must have valid bus, dev & fn)
 * @param regOffset The byte offset from the Config space of the device
 * @return The byte data from that offset
 */
static uint8_t _pci_read_config_byte(pci_dev_st* dev, uint8_t regOffset) {
    outl(PCI_CONFIG_ADDR_PORT, PCI_CONFIG_ADDR_BIT | PCI_ADDR(dev->busNum, dev->devNum, dev->functNum, regOffset));

    io_wait();

    uint32_t resp = inl(PCI_CONFIG_DATA_PORT);
    return (uint8_t)(resp >> ((regOffset % 4) * 8));
}

/**
 * Writes dword data to the device's configuration space at the specified
 * register offset.
 * @param data The double word to write to the space
 * @param dev Pointer to PCI device struct (must have valid bus, dev & fn)
 * @param regOffset The register offset from the config space
 * @return None
 */
static void _pci_write_config_dword(uint32_t data, pci_dev_st* dev, uint8_t regOffset) {
    outl(PCI_CONFIG_ADDR_PORT, PCI_CONFIG_ADDR_BIT | PCI_ADDR(dev->busNum, dev->devNum, dev->functNum, regOffset));
    io_wait();
    
    outl(PCI_CONFIG_DATA_PORT, data);
    io_wait();
}

/**
 * Writes word data to the device's configuration space at the specified
 * register offset.
 * @param data The word to write to the space
 * @param dev Pointer to PCI device struct (must have valid bus, dev & fn)
 * @param regOffset The register offset from the config space
 * @return None
 */
static void _pci_write_config_word(uint16_t data, pci_dev_st* dev, uint8_t regOffset) {
    outl(PCI_CONFIG_ADDR_PORT, PCI_CONFIG_ADDR_BIT | PCI_ADDR(dev->busNum, dev->devNum, dev->functNum, regOffset));
    io_wait();

    uint32_t configData = inl(PCI_CONFIG_DATA_PORT);
    io_wait();

    int bitShift = (regOffset & 0x2) * 8;
    configData &= (0xFFFF0000 >> bitShift);

    uint32_t shiftedData = data;
    configData |= (shiftedData << bitShift);
    
    outl(PCI_CONFIG_DATA_PORT, configData);
    io_wait();
}

/**
 * Writes byte data to the device's configuration space at the specified
 * register offset.
 * @param data The byte to write to the space
 * @param dev Pointer to PCI device struct (must have valid bus, dev & fn)
 * @param regOffset The register offset from the config space
 * @return None
 */
static void _pci_write_config_byte(uint8_t data, pci_dev_st* dev, uint8_t regOffset) {
    outl(PCI_CONFIG_ADDR_PORT, PCI_CONFIG_ADDR_BIT | PCI_ADDR(dev->busNum, dev->devNum, dev->functNum, regOffset));
    io_wait();
    
    uint32_t configData = inl(PCI_CONFIG_DATA_PORT);
    io_wait();

    switch (regOffset % 4) {
        case 1:
        configData &= 0xFFFF00FF;
        case 2:
        configData &= 0xFF00FFFF;
        case 3:
        configData &= 0x00FFFFFF;
        default:
        configData &= 0xFFFFFF00;
    }
    
    uint32_t shiftedData = data;
    configData |= (shiftedData << (regOffset % 4) * 8);
    
    outl(PCI_CONFIG_DATA_PORT, configData);
    io_wait();
}

/**
 * Obtains PCI Configuration Header information for the device.
 * NOTE: Invalid device implies all information is set to all 1's
 * @param devHdr Pointer to PCI device header struct to store all information
 * @param dev Pointer to PCI device struct
 * @return None 
 */
void pciGetHeaderInfo(pci_dev_hd_st* devHdr, pci_dev_st* dev) {
    // Get Device & Vendor ID
    uint32_t configData = _pci_read_config_dword(dev, 0x0);
    devHdr->vendorId = (uint16_t)(configData & 0x0000FFFF);
    devHdr->deviceId = (uint16_t)((configData >> 16) & 0x0000FFFF);

    // Get Class & Revision info
    configData = _pci_read_config_dword(dev, 0x8);
    devHdr->revId = (uint8_t)(configData & 0x000000FF);
    devHdr->interface = (uint8_t)((configData >> 8) & 0x000000FF);
    devHdr->subClass = (uint8_t)((configData >> 16) & 0x000000FF);
    devHdr->baseClass = (uint8_t)((configData >> 24) & 0x000000FF);

    // Get Header Type info
    devHdr->headerType = _pci_read_config_byte(dev, 0xE);
}

/**
 * Check if PCI location specified by the device struct can exist.
 * @param dev Pointer to PCI device struct
 * @return True if valid, False otherwise
 */
bool pciHasValidLocation(pci_dev_st* dev) {
    if (dev->devNum < 0 || dev->devNum >= 32) {
        kerror(KERN_INFO, "Invalid PCI device #: %d\n", dev->devNum);
        return false;
    }
    
    if (dev->functNum < 0 || dev->functNum >= 8) {
        kerror(KERN_INFO, "Invalid PCI function #: %d\n", dev->functNum);
        return false;
    }

    return true;
}

/**
 * Updates PCI status word for the device via the struct.
 * NOTE: Invalid device implies all information is set to all 1's
 * @param dev Pointer to PCI device struct
 * @return None 
 */
void pciGetDevStatus(pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return;
    
    dev->status = _pci_read_config_word(dev, 0x6);
}

/**
 * Updates PCI command word for the device via the struct.
 * NOTE: Invalid device implies all information is set to all 1's
 * @param dev Pointer to PCI device struct
 * @return None 
 */
void pciGetDevCmd(pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return;
    
    dev->command = _pci_read_config_word(dev, 0x4);
}

/**
 * Updates PCI status for the device via the struct.
 * NOTE: Invalid device implies all information is set to all 1's
 * @param dev Pointer to PCI device struct
 * @return None 
 */
bool pciDoBist(pci_dev_st* dev) {
    // Basic dev sanity check
    if (!pciHasValidLocation(dev))
        return false;

    // Check if device exists (if not, should return all 1s)
    uint8_t bistStatus = _pci_read_config_byte(dev, 0xF);
    if (bistStatus == 0xFF) {
        kerror(KERN_INFO, "PCI dev %d:%d.%d does not exist!\n", dev->busNum, dev->devNum, dev->functNum);
        return false;
    }

    // Check if device supports BIST
    dev->bist = (bistStatus & 0x80 == 1);

    if (!dev->bist) {
        kerror(KERN_INFO, "PCI device does not support BIST!\n");
        return false;
    }

    // Check if device is already doing a BIST test, if not begin one
    if (bistStatus & 0x40 == 0) {
        kerror(KERN_DEBUG, "PCI dev %d:%d.%d self test started!\n", dev->busNum, dev->devNum, dev->functNum);
        _pci_write_config_byte(bistStatus | 0x40, dev, 0xF);
    } else {
        kerror(KERN_DEBUG, "PCI dev %d:%d.%d self test already in progress!\n", dev->busNum, dev->devNum, dev->functNum);
    }

    // Wait until start BIST is cleared or times out (~2s)
    uint16_t oldCount = getTimerCount();
    bool completed = false;
    do {
        io_wait();
        completed = (_pci_read_config_byte(dev, 0xF) & 0x40 == 0);
    } while (!completed && getCycleElapsed(oldCount) < 73);
    
    // Check if BIST was completed
    if (!completed) {
        kerror(KERN_INFO, "PCI dev %d:%d.%d self test timed out!\n", dev->busNum, dev->devNum, dev->functNum);
        return false;
    }

    // Look at completion status
    bistStatus = _pci_read_config_byte(dev, 0xF);
    if (bistStatus & 0x0F == 0) {
        kerror(KERN_INFO, "PCI dev %d:%d.%d self test completed!\n", dev->busNum, dev->devNum, dev->functNum);
        return true;
    } else {
        kerror(KERN_INFO, "PCI dev %d:%d.%d self test failed!\n", dev->busNum, dev->devNum, dev->functNum);
        return false;
    }
}

/**
 * Check specified dev has MMIO enabled for PCI.
 * @param dev Pointer to PCI device struct
 * @return True if enabled, False if device does not exist or has it disabled
 */
bool pciMemIoEnabled(pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return false;
    
    uint16_t cmd = _pci_read_config_word(dev, 0x4);
    if (cmd == 0xFFFF)
        return false;
    
    return (cmd & 0x2 != 0);
}

/**
 * Check specified dev has IO enabled for PCI.
 * @param dev Pointer to PCI device struct
 * @return True if enabled, False if device does not exist or has it disabled
 */
bool pciIoEnabled(pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return false;
    
    uint16_t cmd = _pci_read_config_word(dev, 0x4);
    if (cmd == 0xFFFF)
        return false;
    
    return (cmd % 2 == 1);
}

/**
 * Checks if specified dev has a parity error.
 * @param dev Pointer to PCI device struct
 * @return True if parity error, False otherwise
 */
bool pciDevHasParityError(pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return false;
    
    uint16_t status = _pci_read_config_word(dev, 0x6);
    if (status == 0xFFFF)
        return false;

    return (status & 0x8000 != 0);
}

/**
 * Checks if specified dev signalled a system error.
 * @param dev Pointer to PCI device struct
 * @return True if signalled, False otherwise
 */
bool pciDevHasSysError(pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return false;
    
    uint16_t status = _pci_read_config_word(dev, 0x6);
    if (status == 0xFFFF)
        return false;

    return (status & 0x4000 != 0);
}

/**
 * Checks if specified dev received abort from Master.
 * @param dev Pointer to PCI device struct
 * @return True if received, False otherwise
 */
bool pciDevHasMasterAbort(pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return false;
    
    uint16_t status = _pci_read_config_word(dev, 0x6);
    if (status == 0xFFFF)
        return false;

    return (status & 0x2000 != 0);
}

/**
 * Checks if specified dev received abort from Target.
 * @param dev Pointer to PCI device struct
 * @return True if received, False otherwise
 */
bool pciDevHasTargetAbort(pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return false;
    
    uint16_t status = _pci_read_config_word(dev, 0x6);
    if (status == 0xFFFF)
        return false;

    return (status & 0x1000 != 0);
}

/**
 * Checks if specified dev signalled an abort.
 * @param dev Pointer to PCI device struct
 * @return True if signalled, False otherwise
 */
bool pciDevHasSignaledAbort(pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return false;
    
    uint16_t status = _pci_read_config_word(dev, 0x6);
    if (status == 0xFFFF)
        return false;

    return (status & 0x0800 != 0);
}

/**
 * Checks if specified dev has Back-Back transactions.
 * @param dev Pointer to PCI device struct
 * @return True if allowed, False otherwise
 */
bool pciDevHasBackToBack(pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return false;
    
    uint16_t status = _pci_read_config_word(dev, 0x6);
    if (status == 0xFFFF)
        return false;

    return (status & 0x0100 != 0);
}

/**
 * Checks if specified dev has raised an interrupt & is enabled.
 * @param dev Pointer to PCI device struct
 * @return True if raised & enabled, False otherwise
 */
bool pciDevIntAsserted(pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return false;
    
    uint16_t status = _pci_read_config_word(dev, 0x6);
    if (status == 0xFFFF)
        return false;

    uint16_t cmd = _pci_read_config_word(dev, 0x4);

    return (status & 0x0008 != 0) && (cmd & 0x4000 == 0);
}

/**
 * Enable / disable interrupts for the specified device.
 * NOTE: Not guaranteed to be set if hardwired to disabled
 * @param enabled True if want to enable, False to disable
 * @param dev Pointer to PCI device struct
 * @return None
 */
void pciToggleInt(bool enabled, pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return;
    
    uint16_t cmd = _pci_read_config_word(dev, 0x4);
    cmd &= 0xFBFF;

    if (!enabled)
        cmd |= 0x0400;

    _pci_write_config_word(cmd, dev, 0x4);
}

/**
 * Enable / disable MMIO for the specified device.
 * NOTE: Not guaranteed to be set if hardwired to disabled
 * @param enabled True if want to enable, False to disable
 * @param dev Pointer to PCI device struct
 * @return None
 */
void pciToggleMemIo(bool enabled, pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return;
    
    uint16_t cmd = _pci_read_config_word(dev, 0x4);
    cmd &= 0xFFFD;

    if (enabled)
        cmd |= 0x0002;

    _pci_write_config_word(cmd, dev, 0x4);
}

/**
 * Enable / disable PCI IO for the specified device.
 * NOTE: Not guaranteed to be set if hardwired to disabled
 * @param enabled True if want to enable, False to disable
 * @param dev Pointer to PCI device struct
 * @return None
 */
void pciToggleIo(bool enabled, pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return;
    
    uint16_t cmd = _pci_read_config_word(dev, 0x4);
    cmd &= 0xFFFE;

    if (enabled)
        cmd |= 0x0001;

    _pci_write_config_word(cmd, dev, 0x4);
}

/**
 * Set the IRQ (for PIC) for the specified device.
 * @param newIrq New IRQ to set for the device to trigger
 * @param dev Pointer to PCI device struct
 * @return None
 */
void pciSetIntLine(uint8_t newIrq, pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return;
    
    _pci_write_config_byte(newIrq, dev, 0x3C);
}

/**
 * Get the current IRQ# for the specified device.
 * NOTE: Can be 0 if not set for the device yet
 * 
 * Return 0xFF for no connection
 * @param dev Pointer to PCI device struct
 * @return IRQ# from dev's config space, 0xFF for invalid
 */
uint8_t pciGetIntLine(pci_dev_st* dev) {
    if (!pciHasValidLocation(dev))
        return 0xFF;

    return _pci_read_config_byte(dev, 0x3C);
}

/**
 * Print the PCI device information
 * @param devHdr Pointer to PCI device header struct
 * @param dev Pointer to PCI device struct
 * @return None
 */
void pciPrintDevInfo(pci_dev_hd_st* devHdr, pci_dev_st* dev) {
    if (!pciHasValidLocation(dev) || devHdr->vendorId == 0xFFFF || devHdr->vendorId == 0x000) {
        kprintf("PSI dev %d:%d.%d does not exist!\n", dev->busNum, dev->devNum, dev->functNum);
        return;
    }

    if (devHdr->vendorId == 0x0001) {
        kprintf("PSI dev %d:%d.%d not ready!\n", dev->busNum, dev->devNum, dev->functNum);
        return;
    }

    kprintf("PSI dev %d:%d.%d:\n", dev->busNum, dev->devNum, dev->functNum);
    kprintf("\tDevice ID: %x, Vendor ID: %x\n", devHdr->deviceId, devHdr->vendorId);
    kprintf("\tClass: %x, subclass: %x, progIf: %x\n", devHdr->baseClass, devHdr->subClass, devHdr->interface);
    kprintf("\tHeader type: %x\n", devHdr->headerType);
}

/**
 * Retrieve the bar address and calculate size for each BAR dev has.
 * 
 * NOTE: If no MMIO bars / IO bars were found, the corresponding access bit in CMD byte should be changed.
 * It is possible that the bit might not be changed.
 * (ie. if no MMIO bars, MMIO accessing is disabled)
 * @param devHdr Pointer to PCI device header struct
 * @param dev Pointer to PCI device struct
 * @return None
 */
void pciDevGetBarInfo(pci_dev_hd_st* devHdr, pci_dev_st* dev) {
    int maxBars = 6;
    if (devHdr->headerType & 0x7F == 0x1) {
        maxBars = 2;
    } else if (devHdr->headerType & 0x7F == 0x2) {
        maxBars = 0;
    }

    bool hasMmio = false;
    bool hasIo = false;

    for (int barNum = 0; barNum < maxBars; barNum++) {
        uint8_t regOffset = 0x10 + (barNum * 4);
    
        uint32_t savedBar = _pci_read_config_dword(dev, regOffset);

        // Get type and address of BAR
        dev->bars[barNum].mmio = (savedBar % 2 == 0);
        dev->bars[barNum].longSize = ((savedBar % 2 == 0) & (savedBar & 0x00000004 == 0x4));
        dev->bars[barNum].prefetch = ((savedBar % 2 == 0) && (savedBar & 0x00000008 == 0x8));
        dev->bars[barNum].address = (savedBar % 2 == 0)? (savedBar & 0xFFFFFFF0) : (savedBar & 0xFFFFFFFC); // NOTE: x86 does not have 64-bit addressing!
        
        uint32_t bar = savedBar;
        
        // Disable decoding in command byte
        uint16_t cmd = _pci_read_config_word(dev, 0x4);
        _pci_write_config_word(cmd & 0xFFFC, dev, 0x4);
        
        // PCI says to write all 1s, flip all bits and add 1 to it to get the size
        _pci_write_config_dword(0xFFFFFFFF, dev, regOffset);
        bar = _pci_read_config_dword(dev, regOffset);
        bar = (savedBar % 2 == 0)? (bar & 0xFFFFFFF0) : (bar & 0xFFFFFFFC);
        uint32_t barSize = (~bar) + 1;

        dev->bars[barNum].size = (bar < 16)? 0 : barSize;

        // Restore old register values
        _pci_write_config_dword(savedBar, dev, regOffset);
        _pci_write_config_word(cmd, dev, 0x4);

        if (dev->bars[barNum].size > 0) {
            if (dev->bars[barNum].mmio) {
                hasMmio = true;
            } else {
                hasIo = true;
            }
        }
    }

    pciToggleMemIo(hasMmio, dev);
    pciToggleIo(hasIo, dev);
}

/**
 * TODO: Add setup functions to this 
 */ 
static void _pci_setup_device(pci_dev_hd_st* devHdr, pci_dev_st* dev) {
    switch (devHdr->baseClass) {

        // Mass storage controller
        case 0x1:
            uint8_t subClass = devHdr->subClass;
            switch (subClass) {
                case 0x1:
                    // call function for IDE
                    break;
                case 0x2:
                    // None for floppy yet
                    break;
                case 0x5:
                    // None for ATA DMA yet
                    break;
                case 0x6:
                    // call function for SATA
                    break;
            }
            break;

        default:
            break;
    }
}

/**
 * Print bar's value, size and if MMIO / IO is used for it
 * 
 * @param devHdr Pointer to PCI device header struct
 * @param dev Pointer to PCI device struct
 * @return None 
 */
void pciPrintBarInfo(pci_dev_hd_st* devHdr, pci_dev_st* dev) {
    int maxBars = 6;
    if (devHdr->headerType & 0x7F == 0x1) {
        maxBars = 2;
    } else if (devHdr->headerType & 0x7F == 0x2) {
        maxBars = 0;
    }

    kprintf("PSI dev %d:%d.%d bars:\n", dev->busNum, dev->devNum, dev->functNum);
    for (int barNum = 0; barNum < maxBars; barNum++) {
        kprintf("\tbar %d) (long: %d)addr: %x, size: %x, mmio?: %d\n", 
                barNum, 
                dev->bars[barNum].longSize, dev->bars[barNum].address, 
                dev->bars[barNum].size, dev->bars[barNum].mmio);
        kprintf("\tBar %d: %x\n", barNum, _pci_read_config_dword(dev, 0x10 + (barNum * 4)));
    }
}

/**
 * Retrieves the rest of the device information (not covered by header)
 * @param devHdr Pointer to PCI device header struct
 * @param dev Pointer to PCI device struct
 * @return None
 */
void pciDevGetInfo(pci_dev_hd_st* devHdr, pci_dev_st* dev) {
    // Bar data
    pciDevGetBarInfo(devHdr, dev);

    // Get status & command words
    uint32_t configData = _pci_read_config_dword(dev, 0x4);
    dev->command = (uint16_t)(configData & 0x0000FFFF);
    dev->status = (uint16_t)((configData >> 16) & 0x0000FFFF);

    // Get cache size & latency
    configData = _pci_read_config_dword(dev, 0xC);
    dev->cacheSize = (uint8_t)(configData & 0x000000FF);
    dev->latency = (uint8_t)((configData >> 8) & 0x000000FF);

    // Get whether BIST is allowed
    uint8_t bistByte = (uint8_t)((configData >> 8) & 0x000000FF);
    dev->bist = (bistByte & 0x80 != 0);

    // Interrupt (PIC IRQ) number
    dev->intLine = _pci_read_config_byte(dev, 0x3C);

    // Check if mmio & io are enabled
    dev->mmioEnabled = ((dev->command & 0x2) != 0);
    dev->ioEnabled = ((dev->command % 2) == 1);
}

/**
 * Iterates through the bus to retrieve all device information
 * @return None
 */
void pciEnumerate() {
    pci_dev_st dev = { 0 };
    pci_dev_hd_st devHr = { 0xFFFF };

    for (uint16_t busNum = 0; busNum < 256; busNum++) {
        for (uint8_t devNum = 0; devNum < 32; devNum++) {
            dev.busNum = busNum;
            dev.devNum = devNum;
            dev.functNum = 0;
            dev.ioEnabled = false;
            dev.mmioEnabled = false;

            // Check if device exists at this location
            uint16_t vendorId = _pci_read_config_word(&dev, 0);

            if (vendorId == 0xFFFF)
                continue;

            pciGetHeaderInfo(&devHr, &dev);
            pciDevGetInfo(&devHr, &dev);
            _pci_setup_device(&devHr, &dev);

            kprintf("PSI dev %d:%d.%d found! Class: %x, sublass %x, interface: %x\n\tio?: %d, mmio?: %d\n", 
                    dev.busNum, dev.devNum, dev.functNum, devHr.baseClass, devHr.subClass, devHr.interface,
                    dev.ioEnabled, dev.mmioEnabled);

            // Device exists, let's check if it only has 1 function
            if (devHr.headerType & 0x80 == 0)
                continue;

            // Device is multifunction, we check all possible ones
            for (uint8_t functNum = 1; functNum < 8; functNum++) {
                dev.functNum = functNum;
                vendorId = _pci_read_config_word(&dev, 0);

                if (vendorId == 0xFFFF)
                    continue;

                pciGetHeaderInfo(&devHr, &dev);
                pciDevGetInfo(&devHr, &dev);
                _pci_setup_device(&devHr, &dev);

                kprintf("PSI dev %d:%d.%d found! Class: %x, sublass %x, interface: %x\n\tio?: %d, mmio?: %d\n", 
                    dev.busNum, dev.devNum, dev.functNum, devHr.baseClass, devHr.subClass, devHr.interface,
                    dev.ioEnabled, dev.mmioEnabled);
            }
        }
    }

    kprintf("PCI enumeration done!\n");
}
