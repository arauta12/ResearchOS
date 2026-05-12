#include <arch/i386/pic.h>

static inline u8 _get_master_imr() { return inb(PIC_MASTER_DATA); }

static inline u8 _get_slave_imr() { return inb(PIC_SLAVE_DATA); }

static inline u16 _get_pic_imr() {
    u16 mask = _get_slave_imr();
    io_wait();
    return (mask << 8) | _get_master_imr();
}

static void _set_pic_imr(u16 mask) {
    u8 master_mask = mask;
    u8 slave_mask = (mask >> 8);
    outb(PIC_MASTER_DATA, master_mask);
    io_wait();
    outb(PIC_SLAVE_DATA, slave_mask);
    io_wait();
}

void pic_init() {
    // Send ICW1s
    outb(PIC_MASTER_COMMAND, ICW1_INIT | ICW1_NEED_ICW4);
    io_wait();
    outb(PIC_SLAVE_COMMAND, ICW1_INIT | ICW1_NEED_ICW4);
    io_wait();

    // Send ICW2s
    outb(PIC_MASTER_DATA, PIC_MASTER_OFFSET);
    io_wait();
    outb(PIC_SLAVE_DATA, PIC_SLAVE_OFFSET);
    io_wait();

    // Send ICW3s
    outb(PIC_MASTER_DATA, PIC_CONNECT_IR(2));
    io_wait();
    outb(PIC_SLAVE_DATA, 2);
    io_wait();

    // Send ICW4s
    outb(PIC_MASTER_DATA, ICW4_PM_86);
    io_wait();
    outb(PIC_SLAVE_DATA, ICW4_PM_86);
    io_wait();

    pic_disable();
}

void pic_disable() { _set_pic_imr(0xFFFF); }

void pic_eoi(u8 irq_num) {
    if (irq_num >= 8) {
        outb(PIC_SLAVE_COMMAND, PIC_EOI);
        io_wait();
    }

    outb(PIC_MASTER_COMMAND, PIC_EOI);
    io_wait();
}

void pic_disable_irq(u8 irq_num) {
    u16 mask = _get_pic_imr();

    _set_pic_imr(mask | (1 << irq_num));
}

void pic_enable_irq(u8 irq_num) {
    u16 mask = _get_pic_imr();
    if (irq_num >= 8) mask &= ~(1 << 2);

    _set_pic_imr(mask & ~(1 << irq_num));
}
