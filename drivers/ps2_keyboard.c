// #include <device/ps2_keyboard.h>
// #include <device/ps2.h>
// #include <stdio.h>

// bool ps2KeyboardInit() {
//     bool success = setScancodeSet(1);
//     return success;
// }

// uint8_t getScancode() {
//     outb(PS2_CMD_PORT, PS2_KEYBOARD_CURRENT_SET);
//     io_wait();
//     return inb(PS2_DATA_PORT);
// }

// uint8_t getScancodeSet() {
//     outb(PS2_KEYBOARD_SCANCODE_SET, PS2_KEYBOARD_CURRENT_SET);
//     io_wait();
//     uint8_t resp = inb(PS2_DATA_PORT);
//     if (resp != 0xFA)
//         return false;

//     io_wait();
//     resp = inb(PS2_DATA_PORT);

//     return resp;
// }

// bool setScancodeSet(int setNum) {
//     outb(PS2_CMD_PORT, PS2_KEYBOARD_SCANCODE_SET);
//     // while (inb(PS2_CMD_PORT) & 1 == 0); 
//     io_wait();
//     inb(PS2_DATA_PORT);
//     inb(PS2_DATA_PORT);
//     outb(PS2_DATA_PORT, setNum);
//     bool success = _status_clear_wait();
//     if (!success) {
//         kprintf("SET SCANS wait failed!\n");
//     }
//     io_wait();
//     inb(PS2_DATA_PORT);
//     io_wait();
//     uint8_t resp = inb(PS2_DATA_PORT);
//     kprintf("SET SCANS resp: %x\n", resp);
//     if (resp != 0xFA)
//         return false;

//     return true;
// }
