#include <device/ps2_keyboard.h>
#include <device/ps2.h>
#include <device/pic.h>
#include <stdio.h>
#include <conversion.h>

static int _current_scancode_set = 0;
static bool _caps_lock = false;
static bool _shift_press = false;

void flushKeyBuffer() {
    while (ps2GetData() != 0);
}

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
            kerror(KERN_ERR, "PS2 Keyboard failed to echo!\n");
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
        if (!ps2SendPort1DataAck(PS2_KEYBOARD_SCANCODE) || !ps2SendPort1DataAck(0x0)) {
            attempts++;
            continue;
        } else {
            return ps2GetData();
        }
    } while (attempts < MAX_ATTEMPTS);

    kerror(KERN_INFO, "PS2 Keyboard failed to get scancodes!\n");
    return 0;
}

bool ps2KeyboardSetScancodeSet(uint8_t setNum) {
    if (setNum != 1 && setNum != 2) {
        kerror(KERN_WARNING, "PS2 Keyboard set %d not supported!\n", setNum);
        return false;
    }

    const int MAX_ATTEMPTS = 3;
    int attempts = 0;

    do {
        if (!ps2SendPort1DataAck(PS2_KEYBOARD_SCANCODE) || !ps2SendPort1DataAck(setNum)) {
            attempts++;
            continue;
        } else {
            return true;
        }
    } while (attempts < MAX_ATTEMPTS);
    
    kerror(KERN_INFO, "PS2 Keyboard failed to change scancode set.\n");
    return false;
}

bool ps2KeyboardSetTypeRate(uint8_t typeByte) {
    const int MAX_ATTEMPTS = 3;
    int attempts = 0;

    do {
        if (!ps2SendPort1DataAck(PS2_KEYBOARD_TYPE_OPT) || !ps2SendPort1DataAck(typeByte)) {
            attempts++;
            continue;
        } else {
            return true;
        }
    } while (attempts < MAX_ATTEMPTS);
    
    kerror(KERN_INFO, "PS2 Keyboard failed to change type rate!\n");
    return false;
}

bool ps2KeyboardToggleScan(bool enable) {
    const int MAX_ATTEMPTS = 3;
    int attempts = 0;

    do {
        if (!ps2SendPort1DataAck(PS2_KEYBOARD_ENABLE + (!enable))) {
            return false;
        } else {
            return true;
        }
    } while (attempts < MAX_ATTEMPTS);
    
    kerror(KERN_INFO, "PS2 Keyboard failed to toggle enable/disable.\n");
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
    
    kerror(KERN_ERR, "PS2 Keyboard failed to set default options.\n");
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
    
    kerror(KERN_ERR, "PS2 Keyboard failed self test.\n");
    return false;
}

static uint8_t _lower_to_upper_letter(uint8_t letter) {
    if (letter >= 'a' && letter <= 'z')
        return letter - 0x20;

    switch (letter) {
        case '1':
            return '!';
        case '2':
            return '@';
        case '3':
            return '#';
        case '4':
            return '$';
        case '5':
            return '%';
        case '6':
            return '^';
        case '7':
            return '&';
        case '8':
            return '*';
        case '9':
            return '(';
        case '0':
            return ')';
        case '-':
            return '_';
        case '=':
            return '+';
        case '[':
            return '{';
        case ']':
            return '}';
        case '\\':
            return '|';
        case '\'':
            return '"';
        case ';':
            return ':';
        case '`':
            return '~';
        case ',':
            return '<';
        case '.':
            return '>';
        case '/':
            return '?';
        default:
            return letter;
    }
}


KEYCHAR irqKeyboard1GetChar() {

}

// FIXME ISSUE
KEYCHAR irqKeyboard2GetChar() {
    KEYCHAR keyInput = {false, 0xff};
    uint8_t scancode = inb(PS2_DATA_PORT);

    if (scancode == 0xF0) {
        scancode = ps2GetData();
        if (scancode == 0x12 ||scancode == 0x59) {
            // Detected left/right shift release!
            _shift_press = false;
            return keyInput;
        }
    } else {
        // Key press down
        keyInput.pressedDown = true;
        if (scancode == 0xE0) {

        } else if (scancode == 0x12 || scancode == 0x59) {
            // pressed Left/right shift keys
            _shift_press = true;
        } else if (scancode == 0x58) {
            // pressed CapsLock
            _caps_lock = !_caps_lock;
        } else if (scancode < 0x80) {
            uint8_t letter = _scancode_set_2[scancode];
            if (_caps_lock)
                letter = toUpper(letter);   // Note: returns same key for non english letters
            
            if (_shift_press) {
                if (isAsciiChar(letter)) {
                    letter = (_caps_lock)? toLower(letter) : toUpper(letter);
                } else {
                    letter = _lower_to_upper_letter(letter);
                }
            }

            keyInput.letter = letter;
        }
    }

    return keyInput;
}

KEYCHAR irqGetKeyboardChar() {
    KEYCHAR keyInput = {false, 0xFF};
    if (_current_scancode_set == 2) {
        return irqKeyboard2GetChar();
    } else if (_current_scancode_set == 1) {
        return keyInput;
    } else {
        return keyInput;
    }
}

// Assumes PS2 config done including this keyboard 
bool ps2KeyboardConfig() {

    kprintf("Configuring PS/2 keyboard...\n");
    while(ps2GetData() != 0x0);

    // small detection test
    if (!ps2KeyboardEcho()) {
        kerror(KERN_ERR, "Keyboard config: failed to detect!\n");
        return false;
    }

    // change to set 2
    if (!ps2KeyboardSetScancodeSet(2)) {
        kerror(KERN_ERR, "Keyboard config: failed to set scancode!\n");
        return false;
    }

    _current_scancode_set = 2;

    // setting default type rate
    if (!ps2KeyboardSetTypeRate(0x60)) {
        kerror(KERN_ERR, "Keyboard config: failed to set type rate!\n");
        return false;
    }

    if (!ps2KeyboardToggleScan(true)) {
        kerror(KERN_ERR, "Keyboard config: failed to enable scanning!\n");
        return false;
    }

    clearMaskIrq(1);
    kprintf("PIC mask w/ keyboard: %x.\n", getPicMask());

    kprintf("PS/2 Keyboard configuration complete!\n");
    return true;
}
