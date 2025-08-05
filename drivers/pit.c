#include <device/pit.h>

static uint16_t _count_value = 0x1000;     // In PIT, this rotates to 0xffff + 1
static int _current_counter = -1;
static int _current_mode = -1;

static uint16_t _read_latch() {
    if (_current_counter < 0)
        return 0xffff;

    uint16_t count = 0;

    outb(PIT_CMD_PORT, PIT_COUNTER(_current_counter));

    io_wait();
    count = inb(PIT_CHANNEL_PORT(_current_counter));
    io_wait();
    count |= (inb(PIT_CHANNEL_PORT(_current_counter)) << 8);

    return count;
}

/* Mainly used for seeing current programmed mode & whether using BCD or not */
static uint8_t _read_status_byte() {
    outb(PIT_CMD_PORT, PIT_RW_CMD | (1 << 5) | (1 << (_current_counter + 1)));
    io_wait();
    return inb(PIT_CHANNEL_PORT(_current_counter));
}

uint16_t getLatchByte() {
    if (_current_counter < 0)
        return 0x0;

    return _read_status_byte();
}

// TODO: sets new value for counter ()
void loadNewCounter() {
    
}

void reloadCounter() {
    if (_current_counter < 0)
        return;

    outb(PIT_CHANNEL_PORT(_current_counter), (uint8_t)(_count_value & 0x00FF));
    io_wait();
    outb(PIT_CHANNEL_PORT(_current_counter), (uint8_t)((_count_value >> 8) & 0x00FF));
}

void pitConfig() {
    outb(PIT_CMD_PORT, PIT_COUNTER(0) | PIT_RW_LSB_MSB | PIT_MODE(3) | PIT_16BIT_MODE);
    _current_counter = 0;
    _current_mode = 3;

    kprintf("PIT configured.\n");
    clearMaskIrq(0);
}
