#include <arch/i386/cpuid_info.h>
#include <arch/i386/isr.h>
#include <arch/i386/pic.h>
#include <arch/i386/ps2.h>
#include <arch/i386/ps2_keyboard.h>
#include <early_kprintf.h>
#include <lib/conversion.h>
#include <main.h>
#include <multiboot2_tbl.h>

void print_mmap() {
    mmap_tbl_st mem_tbl;
    get_mmap(&mem_tbl);

    kputs("Bios memory map:\n");
    mmap_entry_st entry;
    for (size_t i = 0; i < mem_tbl.num_entries; i++) {
        entry = mem_tbl.tbl[i];
        kprintf("[ ENTRY %d ] addr=0x%p, len=0x%p, type=%d\n", i,
                (size_t)entry.addr, (size_t)entry.len, entry.type);
    }
}

void print_fb() {
    fb_info_st fb = {0};
    get_fb_info(&fb);
    kprintf("FB addr: %p, width=%x,height=%x,pitch=%x\n", fb.addr, fb.width,
            fb.height, fb.pitch);
}

void print_kernel_info() {
    kprintf("Biosdev: %x, partition: %x\n", get_biosdev(), get_partition());
    kprintf(
        "Loadaddr: %p\nMem size: 0x%x KB\nBootloader: %s\nKrnl Args: %s\n\n",
        get_loadaddr(), get_total_mem_kb(), get_bootloader_name(),
        get_cmd_arg());
}

// Tasks: memory setup, device setup, setup for init task
void arch_kmain(const void* mb_tbl) {
    tty_init();
    mb2_tbl_init(mb_tbl);

    pic_init();
    ps2_initiate();
    ps2_keyboard_config();

    register_idt_entry(0x2, (u32)&isr_gen_prot_handler, 0, INT_32);
    register_idt_entry(0xD, (u32)&isr_gen_prot_handler, 0, INT_32);
    register_idt_entry(0x8, (u32)&isr_gen_prot_handler, 0, INT_32);
    register_idt_entry(0xE, (u32)&isr_gen_prot_handler, 0, INT_32);

    load_idt();
    enable_int();
    pic_enable_irq(1);

    kmain();
}