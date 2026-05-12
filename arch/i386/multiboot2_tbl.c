#include <multiboot2_tbl.h>

static void* mb2_tbl[NUM_MB2_ENTRIES + 1] = {NULL};
static mod_info_st* mb2_mods[NUM_MB2_MODS] = {0};

void mb2_tbl_init(const void* ptr) {
    struct mb2_tbl_hdr* tbl = (struct mb2_tbl_hdr*)ptr;
    size_t offset = sizeof(struct mb2_tbl_hdr);
    size_t size = tbl->size;
    size_t num_mods = 0;

    struct mb2_tag_hdr* entry = (struct mb2_tag_hdr*)(tbl + 1);
    while (offset < size) {
        if (entry->type == MB2_MOD_TYPE) {
            if (num_mods < NUM_MB2_MODS) {
                struct mb2_modules* mod = (struct mb2_modules*)entry;
                mb2_mods[num_mods] = &mod->info;
            }
            num_mods++;

            mb2_tbl[entry->type] = &mb2_mods;
            offset += entry->size;
            entry = (struct mb2_tag_hdr*)((u8*)entry + entry->size);
        } else if (entry->type >= 1 && entry->type <= NUM_MB2_ENTRIES) {
            mb2_tbl[entry->type] = entry;
            offset += entry->size;
            entry = (struct mb2_tag_hdr*)((u8*)entry + entry->size);
        } else {
            offset += 8;
            entry += 8;
        }

        entry = (struct mb2_tag_hdr*)TAG_ALIGN((size_t)entry);
        offset = TAG_ALIGN(offset);
    }
}

size_t get_total_mem_kb() {
    struct mb2_mem_info* mem_info =
        (struct mb2_mem_info*)mb2_tbl[MB2_MEM_INFO_TYPE];

    if (mem_info == NULL) return 0;

    return (size_t)(mem_info->mem_lower_kb) + (size_t)(mem_info->mem_upper_kb);
}

void get_mmap(mmap_tbl_st* map) {
    struct mb2_mmap* mb_map = (struct mb2_mmap*)mb2_tbl[MB2_MMAP_TYPE];
    if (mb_map == NULL) {
        map->num_entries = 0;
        map->tbl = NULL;
        return;
    }

    map->num_entries =
        (mb_map->hdr.size - sizeof(struct mb2_tag_hdr)) / mb_map->entry_size;

    map->tbl = (mmap_entry_st*)(mb_map + 1);
}

void* get_loadaddr() {
    struct mb2_phy_addr* entry = (struct mb2_phy_addr*)mb2_tbl[MB2_PHY_ADDR];
    if (entry == NULL) return NULL;

    return (void*)entry->addr;
}

char* get_bootloader_name() {
    struct mb2_boot_name* entry =
        (struct mb2_boot_name*)mb2_tbl[MB2_BOOT_NAME_TYPE];

    if (entry == NULL) return NULL;

    return (char*)entry->str;
}

char* get_cmd_arg() {
    struct mb2_boot_name* entry = (struct mb2_boot_name*)mb2_tbl[MB2_CLI_TYPE];

    if (entry == NULL) return NULL;

    return (char*)entry->str;
}

int get_acpi_version() {
    if (mb2_tbl[MB2_RSDPV2_TYPE] != NULL) return 2;
    if (mb2_tbl[MB2_RSDPV1_TYPE] != NULL) return 1;

    return 0;
}

void* get_rsdp() {
    int ver = get_acpi_version();
    struct mb2_rsdp* entry = NULL;
    switch (ver) {
        case 2:
            entry = (struct mb2_rsdp*)mb2_tbl[MB2_RSDPV2_TYPE];
            break;
        case 1:
            entry = (struct mb2_rsdp*)mb2_tbl[MB2_RSDPV1_TYPE];
            break;
        default:
            return NULL;
    }

    return entry->rsdp;
}

void get_smbios(smbios_tbl_st* ptr) {
    struct mb2_smbios_tbl* tbl =
        (struct mb2_smbios_tbl*)mb2_tbl[MB2_SMBIOS_TYPE];

    if (tbl == NULL) return;

    ptr->major = tbl->major;
    ptr->minor = tbl->minor;
    ptr->tbl = (void*)(tbl + 1);
}

void get_apm_info(apm_info_st* ptr) {
    struct mb2_apm* tbl = (struct mb2_apm*)mb2_tbl[MB2_APM_TYPE];
    if (tbl == NULL) return;

    *ptr = tbl->info;
}

mod_info_st** get_modules() { return mb2_mods; }

u32 get_partition() {
    struct mb2_boot_dev* dev = (struct mb2_boot_dev*)mb2_tbl[MB2_BOOT_DEV];
    if (dev == NULL) return 0;

    return dev->partition;
}

u32 get_biosdev() {
    struct mb2_boot_dev* dev = (struct mb2_boot_dev*)mb2_tbl[MB2_BOOT_DEV];
    if (dev == NULL) return 0;

    return dev->biosdev;
}

void get_fb_info(fb_info_st* info) {
    struct mb2_fb_info* dev = (struct mb2_fb_info*)mb2_tbl[MB2_FB_INFO_TYPE];
    if (dev == NULL) return;

    *info = *((fb_info_st*)((u8*)dev + sizeof(struct mb2_tag_hdr)));
}