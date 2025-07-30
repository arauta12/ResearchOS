#ifndef PIT_H
#define PIT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <io.h>
#include <device/pic.h>

/*

# 8254 Programmable Interval Timer (PIT)

## Links
[Intel manual](https://www.scs.stanford.edu/10wi-cs140/pintos/specs/8254.pdf)
*/

#define PIT_CHANNEL_PORT(n)     (0x40 + n)
#define PIT_CMD_PORT            0x43

#define PIT_BCD_MODE        1
#define PIT_16BIT_MODE      0
#define PIT_MODE(n)         (n << 1)  // From 0 to 5
#define PIT_COUNTER_LATCH   0 << 4
#define PIT_RW_LSB          1 << 4
#define PIT_RW_MSB          2 << 4
#define PIT_RW_LSB_MSB      3 << 4
#define PIT_COUNTER(n)      (n << 6)
#define PIT_RW_CMD          3 << 6

static uint16_t _count_value;
static int _current_counter;
static int _current_mode;

static uint16_t _read_latch();
static uint8_t _read_status_byte();
uint16_t getLatchByte();
void loadNewCounter();
void reloadCounter();
void pitConfig();

#endif
