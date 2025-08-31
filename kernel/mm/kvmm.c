#include <mm/kvmm.h>

static kvmm_entry_st* _krnl_vmm_map = NULL;
static kvmm_entry_st* _krnl_vmm_last = NULL;

static uint32_t* _krnl_phy_pdptr = NULL;

uint16_t _get_dir_index(void* addr) {
    return (uint16_t)((uint32_t)addr >> (PAGE_EXP + PG_INDEX_EXP));
}

uint16_t _get_tbl_index(void* addr) {
    uint32_t index = (uint32_t)addr >> PAGE_EXP;
    return (uint16_t)((index << (PAGE_EXP + PG_INDEX_EXP)) >> (PAGE_EXP + PG_INDEX_EXP));
}

uint16_t _get_pg_offset(void* addr) {
    return (uint16_t)(((uint32_t)addr << (PG_INDEX_EXP * 2)) >> (PG_INDEX_EXP * 2));
}

uint32_t _get_pg_index(void* addr) {
    return ((uint32_t)addr >> PAGE_EXP);
}

kvmm_entry_st* getKvmmMap() {
    return _krnl_vmm_map;
}

uint32_t* getPgDirPtr() {
    return _krnl_phy_pdptr;
}

void kvmmInit() {
    _krnl_phy_pdptr = (uint32_t*)pmmAllocPages(1);

    // Setup identity map (temporarily)
    uint8_t* pgAddr = (uint8_t*)0;
    uint8_t* endPgAddr = (uint8_t*)STACK_START;

    for (uint32_t dirIndex = _get_dir_index(pgAddr); dirIndex <= _get_dir_index(endPgAddr); dirIndex++) {
        _krnl_phy_pdptr[dirIndex] = (uint32_t)pmmAllocPages(1) | 0x3;
    }

    while (pgAddr < endPgAddr) {
        uint32_t* currTbl = (uint32_t*)(_krnl_phy_pdptr[_get_dir_index(pgAddr)] & 0xFFFFFFFC);
        currTbl[_get_tbl_index(pgAddr)] = ((uint32_t)pgAddr) | 0x3;

        pgAddr += PAGE_SIZE;
    }

    // Setup Higher-Half Mapping (map 0-STACK_END by adding offset)
    pgAddr = (uint8_t*)_TO_VM(0);
    endPgAddr = (uint8_t*)_TO_VM(STACK_START);

    for (uint32_t dirIndex = _get_dir_index(pgAddr); dirIndex <= _get_dir_index(endPgAddr); dirIndex++) {
        _krnl_phy_pdptr[dirIndex] = (uint32_t)pmmAllocPages(1) | 0x3;
    }

    while (pgAddr < endPgAddr) {
        uint32_t* currTbl = (uint32_t*)(_krnl_phy_pdptr[_get_dir_index(pgAddr)] & 0xFFFFFFFC);
        currTbl[_get_tbl_index(pgAddr)] = _TO_PM((uint32_t)pgAddr) | 0x3;

        pgAddr += PAGE_SIZE;
    }
}

void updateKrnlPgTbl() {

}

void addMmioEntry(void* addr, uint32_t size) {

}
