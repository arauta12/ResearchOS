#ifndef BOOTINFO_H
#define BOOTINFO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <device/drive_main.h>

#define END_TAG_TYPE    0
#define BOOT_CMD_LINE   1
#define BOOTLOADER_NAME 2
#define MODULES         3
#define BASIC_MEM       4
#define BIOS_DEVICE     5
#define MMAP            6
#define VBE_INFO        7
#define FRAMEBUF_INFO   8
#define ELF_SYMBOLS     9
#define APM_TABLE       10
#define EFI_32_SYS_TBL  11
#define EFI_64_SYS_TBL  12
#define SMBIOS_TBL      13
#define ACPI_OLD_RSDP   14
#define ACPI_NEW_RSDP   15
#define NET_INFO        16
#define EFI_MMAP        17
#define EFI_NOT_TERM    18
#define EFI_32_IMG_PTR  19
#define EFI_64_IMG_PTR  20
#define BASE_PHY_ADDR   21

typedef struct {
    uint32_t total_size;
    uint32_t reserved;
} bootinfo_head_st;

typedef struct {
    uint32_t type;
    uint32_t size;
} tag_start_st;

static bootinfo_head_st* _multiboot_tbl_ptr;
static uint32_t _krnl_physical_addr;

void readPhyAddr(tag_start_st* tagPtr);
void readBootDev(tag_start_st* tagPtr);
void readBootCmd(tag_start_st* tagPtr);
void readBootName(tag_start_st* tagPtr);

void readBootTable(const void* tblPtr);
uint32_t getKrnlPhyAddr();

#endif
