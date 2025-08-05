#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <isr.h>

/*

Configuration Address Format:
====================================================================================
| Bit 31 | Bits 30-24 |   Bits 23-16   | Bits 15-10 | Bits 10-8  |     Bits 7-0*   | 
====================================================================================
| Enable |  Reserved  |      Bus #	   |  Device #  | Function # | Register Offset |
====================================================================================
*Note: must be a Dword multiple (bits 1-0 must be 0): this chooses the Dword

Common Header Field:
============================================================
| Bits 31-24 | Bits 23-16 | Bits 15-8 | Bits 7-0 | Offset  |
============================================================
|		 Device ID		  |      Vendor ID       |   0x0   |
------------------------------------------------------------
|		   Status		  |       Command		 |   0x4   |
------------------------------------------------------------
|	Class	 |	Subclass  |	Interface | Revision |   0x8   |
------------------------------------------------------------
|    BIST	 | HeaderType |  Latency  | CacheSize|   0xC   |
------------------------------------------------------------

*/

#define PCI_CONFIG_ADDR_PORT    0xCF8
#define PCI_CONFIG_DATA_PORT    0xCFC
#define PCI_CONFIG_ADDR_BIT     1 << 31
#define PCI_ADDR(b,d,f,r)       ((b << 16) | (d << 11) | (f << 8) | (r & 0xFC))

// For information purposes
typedef struct {
	uint16_t vendorId;
	uint16_t deviceId;
	uint8_t baseClass;
	uint8_t subClass;
	uint8_t interface;
	uint8_t revId;
	uint8_t headerType;
} pci_dev_hd_st;

typedef struct {
	uint32_t address;
	uint32_t size;
	bool prefetch;
	bool longSize;	// True if 64-bit space, false for 32
	bool mmio;
} pci_bar_st;

// For practical / communication purposes
typedef struct {
	pci_bar_st bars[6];
	uint16_t status;
	uint16_t command;
	uint8_t cacheSize;
	uint8_t latency;
	uint8_t intLine;
    uint8_t busNum;
    uint8_t devNum;
    uint8_t functNum;
	bool bist;
	bool ioEnabled;
	bool mmioEnabled;
} pci_dev_st;

// Internal PCI accessing
static uint32_t _pci_read_config_dword(pci_dev_st* dev, uint8_t regOffset);
static uint16_t _pci_read_config_word(pci_dev_st* dev, uint8_t regOffset);
static uint8_t _pci_read_config_byte(pci_dev_st* dev, uint8_t regOffset);

static void _pci_write_config_dword(uint32_t data, pci_dev_st* dev, uint8_t regOffset);
static void _pci_write_config_word(uint16_t data, pci_dev_st* dev, uint8_t regOffset);
static void _pci_write_config_byte(uint8_t data, pci_dev_st* dev, uint8_t regOffset);

static void _pci_setup_device(pci_dev_hd_st* devHdr, pci_dev_st* dev);

void pciGetHeaderInfo(pci_dev_hd_st* devHdr, pci_dev_st* dev);
void pciDevGetBarInfo(pci_dev_hd_st* devHdr, pci_dev_st* dev);
void pciDevGetInfo(pci_dev_hd_st* devHdr, pci_dev_st* dev);

bool pciHasValidLocation(pci_dev_st* dev);

void pciGetDevStatus(pci_dev_st* dev);
void pciGetDevCmd(pci_dev_st* dev);
bool pciDoBist(pci_dev_st* dev);
bool pciMemIoEnabled(pci_dev_st* dev);
bool pciIoEnabled(pci_dev_st* dev);


// Status functions
bool pciDevHasParityError(pci_dev_st* dev);
bool pciDevHasSysError(pci_dev_st* dev);
bool pciDevHasMasterAbort(pci_dev_st* dev);
bool pciDevHasTargetAbort(pci_dev_st* dev);
bool pciDevHasSignaledAbort(pci_dev_st* dev);
bool pciDevHasBackToBack(pci_dev_st* dev);
bool pciDevIntAsserted(pci_dev_st* dev);

// Command functions
void pciToggleInt(bool enabled, pci_dev_st* dev);
void pciToggleMemIo(bool enabled, pci_dev_st* dev);
void pciToggleIo(bool enabled, pci_dev_st* dev);

void pciSetIntLine(uint8_t newIrq, pci_dev_st* dev);
uint8_t pciGetIntLine(pci_dev_st* dev);

void pciPrintDevInfo(pci_dev_hd_st* devHdr, pci_dev_st* dev);
void pciPrintBarInfo(pci_dev_hd_st* devHdr, pci_dev_st* dev);

void pciEnumerate();

#endif
