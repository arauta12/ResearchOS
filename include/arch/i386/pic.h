#pragma once

#include <common.h>
#include <io.h>

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1

#define ICW1_INIT (1 << 4)
#define ICW1_LEVEL_TRIGGER (1 << 3)
#define ICW1_SINGLE (1 << 1)
#define ICW1_NEED_ICW4 (1 << 0)

#define PIC_MASTER_OFFSET 0x20
#define PIC_SLAVE_OFFSET 0x28

#define PIC_EOI 0x20

#define PIC_CONNECT_IR(n) (1 << (n))

#define ICW4_SFNM (1 << 4)
#define ICW4_BUFF (1 << 3)
#define ICW4_MASTER (1 << 2)
#define ICW4_AEOI (1 << 1)
#define ICW4_PM_86 (1 << 0)

static inline u8 _get_master_imr();
static inline u8 _get_slave_imr();
static inline u16 _get_pic_imr();
static inline void _set_pic_imr(u16 mask);

void pic_init();
void pic_disable();
void pic_eoi(u8 irq_num);
void pic_disable_irq(u8 irq_num);
void pic_enable_irq(u8 irq_num);
