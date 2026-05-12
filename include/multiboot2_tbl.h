#pragma once
#include <common.h>
#include <stddef.h>

#define NUM_MB2_ENTRIES 21
#define NUM_MB2_MODS 10
#define TAG_ALIGN(ptr) (((ptr) + 7) & (~7))

struct mb2_tbl_hdr {
    u32 size;
    u32 reserved;
};

struct mb2_tag_hdr {
    u32 type;
    u32 size;
};

/*
    =================
        Boot info
    =================
*/
#define MB2_BOOT_DEV 5
struct mb2_boot_dev {
    struct mb2_tag_hdr hdr;
    u32 biosdev;
    u32 partition;
    u32 sub_partition;
};

#define MB2_PHY_ADDR 21
struct mb2_phy_addr {
    struct mb2_tag_hdr hdr;
    u32 addr;
};

#define MB2_BOOT_NAME_TYPE 2
struct mb2_boot_name {
    struct mb2_tag_hdr hdr;
    u8 str[0];
};

// Also contains a string
#define MB2_CLI_TYPE 1
struct mb2_boot_cli {
    struct mb2_tag_hdr hdr;
    u8 str[0];
};

// Also contains string to module info
#define MB2_MOD_TYPE 3
typedef struct {
    u32 mod_start;
    u32 mod_end;
    u8 data[0];
} mod_info_st;

struct mb2_modules {
    struct mb2_tag_hdr hdr;
    mod_info_st info;
};

// Also contains sections headers
#define MB2_ELF_SYM_TYPE 9
struct mb2_elf_symbols {
    struct mb2_tag_hdr hdr;
    u16 num;
    u16 elf_size;
    u16 shndx;
    u16 reserved;
};

/*
    =================
        Mem info
    =================
*/
#define MB2_MEM_INFO_TYPE 4
struct mb2_mem_info {
    struct mb2_tag_hdr hdr;
    u32 mem_lower_kb;
    u32 mem_upper_kb;
};

#define MB2_MMAP_TYPE 6

// Entries not included here
struct mb2_mmap {
    struct mb2_tag_hdr hdr;
    u32 entry_size;
    u32 entry_version;
};

#define MEM_AVAIL_TYPE 1
#define MEM_RESERVED_TYPE 2
#define MEM_ACPI_TYPE 3
#define MEM_DEFECT_TYPE 4

typedef struct {
    u64 addr;
    u64 len;
    u32 type;
    u32 reserved;
} mmap_entry_st;

typedef struct {
    size_t num_entries;
    mmap_entry_st* tbl;
} mmap_tbl_st;

/*
    ===================
        Device info
    ===================
*/
#define MB2_APM_TYPE 10
typedef struct {
    u16 version;
    u16 cseg;
    u32 offset;
    u16 cseg_16;
    u16 dseg;
    u16 flags;
    u16 cseg_len;
    u16 cseg_16_len;
    u16 dseg_len;
} apm_info_st;

struct mb2_apm {
    struct mb2_tag_hdr hdr;
    apm_info_st info;
};

#define MB2_VBE_TYPE 7
typedef struct {
    u16 mode;
    u16 seg;
    u16 offset;
    u16 len;
    u8 ctrl_info[512];
    u8 mode_info[256];
} vbe_info_st;

struct mb2_vbe_info {
    struct mb2_tag_hdr hdr;
    vbe_info_st info;
};

#define MB2_FB_INFO_TYPE 8
#define MB2_FB_IDX_CLR 0
#define MB2_FB_RGB_CLR 1
#define MB2_FB_EGA 2
struct mb2_color_descr {
    u8 red;
    u8 green;
    u8 blue;
};

// Also contains array of color descriptors
struct mb2_fb_idx_clr {
    u32 num_colors;
};

struct mb2_fb_rgb_clr {
    u8 red_pos;
    u8 red_mask_size;
    u8 green_pos;
    u8 green_mask_size;
    u8 blue_pos;
    u8 blue_mask_size;
};

struct mb2_fb_info {
    struct mb2_tag_hdr hdr;
    u64 addr;
    u32 pitch;
    u32 width;
    u32 height;
    u8 bpp;
    u8 type;
    u8 reserved;
};
typedef struct {
    size_t addr;
    u32 pitch;
    u32 width;
    u32 height;
    u8 bpp;
} fb_info_st;

// Also contains SMBIOS tables after
#define MB2_SMBIOS_TYPE 13
struct mb2_smbios_tbl {
    struct mb2_tag_hdr hdr;
    u8 major;
    u8 minor;
    u8 reserved[6];
};
typedef struct {
    u8 major;
    u8 minor;
    size_t size;
    void* tbl;
} smbios_tbl_st;

#define MB2_RSDPV1_TYPE 14
#define MB2_RSDPV2_TYPE 15
struct mb2_rsdp {
    struct mb2_tag_hdr hdr;
    u8 rsdp[0];
};

// Also contains DHCP ACK
#define MB2_NET_INFO_TYPE 16
struct mb2_net_info {
    struct mb2_tag_hdr hdr;
};

/*
    =================
        EFI info
    =================
*/
#define MB2_EFI32_TBL_TYPE 11
struct mb2_efi32_tbl {
    struct mb2_tag_hdr hdr;
    u32 ptr;
};

#define MB2_EFI64_TBL_TYPE 12
struct mb2_efi64_tbl {
    struct mb2_tag_hdr hdr;
    u64 ptr;
};

#define MB2_EFI_BOOT_TYPE 18
struct mb2_efi_boot {
    struct mb2_tag_hdr hdr;
};

#define MB2_EFI32_IMG_TYPE 19
struct mb2_efi32_img {
    struct mb2_tag_hdr hdr;
    u32 handle;
};

#define MB2_EFI64_IMG_TYPE 20
struct mb2_efi64_img {
    struct mb2_tag_hdr hdr;
    u64 handle;
};

void mb2_tbl_init(const void* ptr);

// Memory info -- might change later
size_t get_total_mem_kb();
void get_mmap(mmap_tbl_st* map);
void* get_loadaddr();

// Boot info
char* get_bootloader_name();
char* get_cmd_arg();
u32 get_partition();
u32 get_biosdev();
void get_fb_info(fb_info_st* info);

// Device info
void get_smbios(smbios_tbl_st* ptr);
void get_apm_info(apm_info_st* ptr);
int get_acpi_version();
void* get_rsdp();

mod_info_st** get_modules();