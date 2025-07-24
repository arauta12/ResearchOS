#include <device/ps2_keyboard.h>
#include <device/ps2.h>
#include <stdio.h>

static int _current_scancode_set = 0;

bool ps2KeyboardEcho() {
    const int MAX_ATTEMPTS = 3;
    int attempts = 0;

    do {
        ps2SendPort1Data(PS2_KEYBOARD_ECHO);
        uint8_t resp = ps2GetData();

        if (resp == PS2_KEYBOARD_ECHO) {
            return true;
        } else if (resp = PS2_KEYBOARD_RESEND_BYTE) {
            attempts++;
        } else {
            kprintf("PS2 Keyboard failed to echo!\n");
            return false;
        }
        
    } while (attempts < MAX_ATTEMPTS);

    return false;
}

uint8_t ps2KeyboardGetScancodeSet() {
    const int MAX_ATTEMPTS = 3;
    int attempts = 0;
    uint8_t resp = 0;

    do {
        if (!ps2SendPort1Data(PS2_KEYBOARD_SCANCODE))
            return 0;

        if (!ps2SendPort1Data(0x0))
            return 0;

        resp = ps2GetData();
        if (resp == PS2_KEYBOARD_RESEND_BYTE) {
            attempts++;
            resp = 0;
        } else {
            return resp;
        }
    } while (attempts < MAX_ATTEMPTS);
    
    kprintf("PS2 Keyboard failed to get scancodes!\n");
    return 0;
}

bool ps2KeyboardSetScancodeSet(uint8_t setNum) {
    if (setNum != 1 && setNum != 2) {
        kprintf("PS2 Keyboard set %d not supported!\n", setNum);
        return false;
    }

    const int MAX_ATTEMPTS = 3;
    int attempts = 0;

    do {
        if (!ps2SendPort1Data(PS2_KEYBOARD_SCANCODE))
            return false;

        if (!ps2SendPort1Data(setNum))
            return false;

        uint8_t resp = ps2GetData();
        if (resp == PS2_KEYBOARD_RESEND_BYTE) {
            attempts++;
        } else if (resp == PS2_KEYBOARD_CMD_ACK){
            return true;
        }
    } while (attempts < MAX_ATTEMPTS);
    
    kprintf("PS2 Keyboard failed to change scancode set.\n");
    return false;
}

bool ps2KeyboardSetTypeRate(uint8_t typeByte) {
    const int MAX_ATTEMPTS = 3;
    int attempts = 0;

    do {
        if (!ps2SendPort1Data(PS2_KEYBOARD_TYPE_OPT))
            return false;

        if (!ps2SendPort1Data(typeByte))
            return false;

        uint8_t resp = ps2GetData();
        if (resp == PS2_KEYBOARD_RESEND_BYTE) {
            attempts++;
        } else if (resp == PS2_KEYBOARD_CMD_ACK){
            return true;
        }
    } while (attempts < MAX_ATTEMPTS);
    
    kprintf("PS2 Keyboard failed to change type rate!\n");
    return false;
}

bool ps2KeyboardToggleScan(bool enable) {
    const int MAX_ATTEMPTS = 3;
    int attempts = 0;

    do {
        if (!ps2SendPort1Data(PS2_KEYBOARD_ENABLE + (!enable)))
            return false;

        uint8_t resp = ps2GetData();
        if (resp == PS2_KEYBOARD_RESEND_BYTE) {
            attempts++;
        } else if (resp == PS2_KEYBOARD_CMD_ACK){
            return true;
        }
    } while (attempts < MAX_ATTEMPTS);
    
    kprintf("PS2 Keyboard failed to toggle enable/disable.\n");
    return false;
}

bool ps2KeyboardSetDefault() {
    const int MAX_ATTEMPTS = 3;
    int attempts = 0;

    do {
        if (!ps2SendPort1Data(PS2_KEYBOARD_DISABLE))
            return false;

        uint8_t resp = ps2GetData();
        if (resp == PS2_KEYBOARD_RESEND_BYTE) {
            attempts++;
        } else if (resp == PS2_KEYBOARD_CMD_ACK){
            return true;
        }
    } while (attempts < MAX_ATTEMPTS);
    
    kprintf("PS2 Keyboard failed to set default options.\n");
    return false;
}

bool ps2KeyboardSelfTest() {
    const int MAX_ATTEMPTS = 3;
    int attempts = 0;

    do {
        if (!ps2SendPort1Data(PS2_KEYBOARD_RESET))
            return false;

        uint8_t resp = ps2GetData();
        if (resp == PS2_KEYBOARD_RESEND_BYTE) {
            attempts++;
        } else if (resp == PS2_KEYBOARD_CMD_ACK){
            resp = ps2GetData();
            break;
        }
    } while (attempts < MAX_ATTEMPTS);

    uint8_t passedByte = ps2GetData();
    if (passedByte == PS2_KEYBOARD_PASS_TEST)
        return true;
    
    kprintf("PS2 Keyboard failed self test.\n");
    return false;
}

KEYCHAR ps2KeyboardGetChar() {
    KEYCHAR keyInput;
    uint8_t resp = ps2GetData();

    if (resp = 0xF0) {
        keyInput.pressedDown = false;
        resp = ps2GetData();
        keyInput.letter = resp;
    } else {
        keyInput.pressedDown = true;
        keyInput.letter = resp;
    }

    return keyInput;
}

KEYCHAR irqGetKeyboardChar() {
    KEYCHAR keyInput;
    keyInput.pressedDown = false;
    keyInput.letter = 0xff;
    uint8_t scancode = inb(PS2_DATA_PORT);

    if (scancode == 0xF0) {
        keyInput.pressedDown = false;
        scancode = ps2GetData();
    } else {
        keyInput.pressedDown = true;
    }
    
    if (_current_scancode_set == 1) {
        if (scancode < 0x58)
            keyInput.letter = _scancode_set_1[scancode];
    } else if (_current_scancode_set == 2) {
        if (scancode < 0x80)
            keyInput.letter = _scancode_set_2[scancode];  
    }

    return keyInput;
}

// Assumes PS2 config done including this keyboard 
bool ps2KeyboardConfig() {

    // small detection test
    if (!ps2KeyboardEcho()) {
        kprintf("Keyboard config: failed to detect!\n");
        return false;
    }

    // change to set 2
    if (!ps2KeyboardSetScancodeSet(2)) {
        kprintf("Keyboard config: failed to set scancode!\n");
        return false;
    }

    _current_scancode_set = 2;

    // setting default type rate
    if (!ps2KeyboardSetTypeRate(0x60)) {
        kprintf("Keyboard config: failed to set type rate!\n");
        return false;
    }

    if (!ps2KeyboardToggleScan(true)) {
        kprintf("Keyboard config: failed to enable scanning!\n");
        return false;
    }

    kprintf("Keyboard config complete!\n");
    return true;
}
