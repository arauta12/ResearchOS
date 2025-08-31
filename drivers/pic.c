#include <device/pic.h>
#include <io.h>
#include <stdio.h>

/*

# PIC (8259)

## Why the PIC?
- Before Intel developed the PIC, x86 computers would require the CPU to regularly poll devices for new data / actions.
- A better way through interrupts was discovered that was more efficient for the CPU: instead of having the CPU waste time sending signals to these devices periodically, you have THEM signal the CPU when they require attention.
- In Intel, this was originally done through the PIC (later upgraded to APIC).

## How does the PIC (generally) work?
1. Devices that raise interrupts are mapped to the PIC through an interrupt line (IRQ #s).
2. A device requesting attention raises the IRQ (i.e. raising the voltage), and the PIC detects this.
3. PIC transforms the IRQ into a system interrupt (which can handle more than devices, i.e. exceptions & faults) via a mapping.
4. If the IRQ has high enough priority, PIC informs the CPU & the CPU handles the interrupt

## 8259 PIC details
### Refer to these for clarification / more details
- [Intel Manual for this](https://pdos.csail.mit.edu/6.828/2010/readings/hardware/8259A.pdf)
- [PIC Wiki article](https://en.wikipedia.org/wiki/Intel_8259)
- [osdev.org article](https://wiki.osdev.org/8259_PIC)
- [Brokenthorn](http://www.brokenthorn.com/Resources/OSDevIndex.html)
### Details
- Each PIC chip has 8 inputs (lines) to receive signals.
- On devices with a single PIC chip, each is connected to an I/O device
- For extended PICs (i.e. two chips), one input is reserved to cascade signals to extended chips (i.e. main chip uses line 2 to signal the 2nd chip).
- If an interrupt is raised, either the main chip sends the CPU interrupt or the extended chip signals the main one on IRQ 2 to raise the interrupt.
- When the CPU requests the interrupt number (vector), the PIC responsible will use an internal offset (programmed in) and the IRQ # to calculate the interrupt vector.

*/

/**
 * Returns mask for both the main & extended PIC devices (IRQs 0-15)
 *
 * Bit n set to 1 means IRQ n is disabled, 0 means enabled
 *
 * Ex. _get_pic_mask() = 0xffaa
 * @param None
 * @return Word value representing the mask for both PIC devices
 */
static uint16_t _get_pic_mask() {
    return (inb(PIC2_DATA) << 8) | inb(PIC1_DATA);
}

/**
 * Returns mask for both the main PIC device (IRQs 0-7)
 *
 * Bit n set to 1 means IRQ n is disabled, 0 means enabled
 *
 * Ex. _get_pic_main_mask() = 0xff
 * @param None
 * @return Byte value representing the mask for main PIC device
 */
static uint8_t _get_pic_main_mask() {
    return inb(PIC1_DATA);
}

/**
 * Returns mask for both the extended PIC device (IRQs 0-7)
 *
 * Bit n set to 1 means IRQ n is disabled, 0 means enabled
 *
 * Ex. _get_pic_ext_mask() = 0xff
 * @param None
 * @return Byte value representing the mask for extended PIC device
 */
static uint8_t _get_pic_ext_mask() {
    return inb(PIC2_DATA);
}

/**
 * Disables all IRQs from both PIC devices
 *
 * @param None
 * @return None
 */
void picDisable() {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

/**
 * Enables all IRQs from both PIC devices
 *
 * @param None
 * @return None
 */
void picEnable() {
    outb(PIC1_DATA, 0);
    outb(PIC2_DATA, 0);
}

/**
 * Reconfigures the PIC to use different IRQ -> interrupt vector offsets
 *
 * NOTE: Make sure the none of the IRQs overlap with predefined
 * interrupt vectors with the new offsets
 *
 * @param mainOffset IRQ offset to use for main PIC device
 * @param extOffset IRQ offset to use for extended PIC device
 * @return None
 */
void picRemap(uint8_t mainOffset, uint8_t extOffset) {
    kprintf("Remapping PIC...\n");

    // ICW1 - basic setup
    outb(PIC1_CMD, ICW1_INIT | ICW1_EDGE_MODE | ICW1_ADI_8 | ICW1_CASCADE | ICW1_NEED_ICW4);
    outb(PIC2_CMD, ICW1_INIT | ICW1_EDGE_MODE | ICW1_ADI_8 | ICW1_CASCADE | ICW1_NEED_ICW4);
    io_wait();

    // ICW2 - IRQ offsets into interrupt table
    outb(PIC1_DATA, mainOffset);
    io_wait();
    outb(PIC2_DATA, extOffset);
    io_wait();

    // ICW3 - cascade connect w/ IRQ 2
    outb(PIC1_DATA, ICW3_EXT(2));
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    // ICW4
    outb(PIC1_DATA, ICW4_NO_NESTED | ICW4_NO_BUFF | ICW4_NORM_EOI | ICW4_8086_MODE);
    io_wait();
    outb(PIC2_DATA, ICW4_NO_NESTED | ICW4_NO_BUFF | ICW4_NORM_EOI | ICW4_8086_MODE);
    io_wait();

    maskIrqs(0xFFFB);
    kprintf("PIC enabled with mask %x.\n", getPicMask());
}

/**
 * Send an End of Interrupt (EOI) to the PIC for the IRQ
 *
 * @param irq The IRQ # to use for the EOI
 * @return None
 */
void picEoi(uint8_t irq) {
    if (irq > 7) {
        outb(PIC2_CMD, PIC_GEN_EOI);
    }

    outb(PIC1_CMD, PIC_GEN_EOI);
}

/**
 * Disable PIC interrupts for the given IRQ number
 *
 * @param irq The IRQ# to disable interrupts for
 * @return None
 */
void setMaskIrq(uint8_t irq) {
    if (irq > 7) {
        uint8_t mask = _get_pic_ext_mask();
        mask |= SET_MASK_IRQ(irq - 8);
        outb(PIC2_DATA, mask);
    } else {
        uint8_t mask = _get_pic_main_mask();
        mask |= SET_MASK_IRQ(irq);
        outb(PIC1_DATA, mask);
    }
}

/**
 * Enable PIC interrupts for the given IRQ number
 *
 * @param irq The IRQ# to enable interrupts for
 * @return None
 */
void clearMaskIrq(uint8_t irq) {
    if (irq > 7) {
        uint8_t mask = _get_pic_ext_mask();
        mask &= ~(SET_MASK_IRQ(irq - 8));
        outb(PIC2_DATA, mask);
    } else {
        uint8_t mask = _get_pic_main_mask();
        mask &= ~(SET_MASK_IRQ(irq));
        outb(PIC1_DATA, mask);
    }
}

/**
 * Apply an interrupt mask for both PIC devices
 *
 * Bit[n] = 1 to disable IRQn, 0 to enable
 *
 * @param mask Word value represent the IRQ mask
 * @return None
 */
void maskIrqs(uint16_t mask) {
    uint8_t mainMask = (mask & 0x00ff);
    uint8_t extMask = (mask & 0xff00) >> 8;
    outb(PIC1_DATA, mainMask);
    io_wait();
    outb(PIC2_DATA, extMask);
}

/**
 * Returns mask for both the main & extended PIC devices (IRQs 0-15)
 *
 * Bit n set to 1 means IRQ n is disabled, 0 means enabled
 *
 * Ex. _get_pic_mask() = 0xffaa
 * @param None
 * @return Word value representing the mask for both PIC devices
 */
uint16_t getPicMask() {
    return _get_pic_mask();
}
