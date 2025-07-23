#ifndef PIC_H
#define PIC_H

#include <stdbool.h>
#include <stdint.h>

/*
    PIC I/O ports
*/

#define PIC1_CMD    0x20
#define PIC1_DATA   0x21

#define PIC2_CMD    0xa0
#define PIC2_DATA   0xa1

/*
    PIC Initialization Command Word 1 (ICW1)
*/
#define ICW1_NEED_ICW4  1
#define ICW1_NO_ICW4    0

#define ICW1_SINGLE     1 << 1
#define ICW1_CASCADE    0 << 1

#define ICW1_ADI_4      1 << 2
#define ICW1_ADI_8      0 << 2

#define ICW1_LVL_MODE   1 << 3
#define ICW1_EDGE_MODE  0 << 3

#define ICW1_INIT       1 << 4


/*
    PIC Initialization Command Word 3 (ICW3)
*/
#define ICW3_EXT(IRQ)   1 << IRQ


/*
    PIC Initialization Command Word 4 (ICW4)
*/
#define ICW4_8086_MODE  1
#define ICW4_8085_MODE  0

#define ICW4_AUTO_EOI   1 << 1
#define ICW4_NORM_EOI   0 << 1

#define ICW4_NO_BUFF    0 << 2
#define ICW4_BUFF_EXT   2 << 2
#define ICW4_BUFF_MAIN  3 << 2

#define ICW4_NESTED     1 << 4
#define ICW4_NO_NESTED  0 << 4


/*
    Operation Command Words (OCWs)
*/

// OCW1 (For Interrupt Masks)
// 1 means masked (inhibit IRQ)
// 0 means enabled
#define SET_MASK_IRQ(IRQ)   1 << IRQ

// OCW2 (For rotate & EOI)
#define PIC_GEN_EOI         1 << 5
#define PIC_SPEC_EOI        3 << 5

// OCW3
#define READ_IRR_REG        0x0a
#define READ_ISR_REG        0x0b

uint8_t _get_pic_main_mask();
uint8_t _get_pic_ext_mask();
uint16_t _get_pic_mask();

void picDisable();
void picEnable();
uint16_t getPicMask();
void picRemap(uint8_t, uint8_t);
void picEoi(uint8_t);
void setMaskIrq(uint8_t);
void clearMaskIrq(uint8_t);
void maskIrqs(uint16_t);

#endif
