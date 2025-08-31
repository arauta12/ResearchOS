#include <device/ps2_keyboard.h>
#include <device/ps2.h>
#include <device/pic.h>
#include <stdio.h>
#include <conversion.h>

static int _current_scancode_set = 0;
static bool _caps_lock = false;
static bool _shift_press = false;
static bool _number_lock = false;

static bool _flush_enabled = false;

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

static uint8_t _lower_to_upper_char(uint8_t letter) {
    if (letter >= 'a' && letter <= 'z')
        return toUpper(letter);

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

static uint8_t _upper_to_lower_char(uint8_t letter) {
    if (letter >= 'A' && letter <= 'Z')
        return toLower(letter);

    switch (letter) {
        case '!':
            return '1';
        case '@':
            return '2';
        case '#':
            return '3';
        case '$':
            return '4';
        case '%':
            return '5';
        case '^':
            return '6';
        case '&':
            return '7';
        case '*':
            return '8';
        case '(':
            return '9';
        case ')':
            return '0';
        case '_':
            return '-';
        case '+':
            return '=';
        case '{':
            return '[';
        case '}':
            return ']';
        case '|':
            return '\\';
        case '"':
            return '\'';
        case ':':
            return ';';
        case '~':
            return '`';
        case '<':
            return ',';
        case '>':
            return '.';
        case '?':
            return '/';
        default:
            return letter;
    }
} 

/**
 * TODO: Do it
 */
key_st irqKeyboard1GetChar() {
    key_st keyInput = {true, 0, UNSUPPORTED_CMD};
    return keyInput;
}

key_st irqKeyboard2GetChar() {
    key_st keyInput = {true, 0, UNSUPPORTED_CMD};
    uint8_t scancode = inb(PS2_DATA_PORT);
    bool additionalCmd = false;

    if (scancode == 0xE1) {
        // Must be pause pressed
        for (int i = 0; i < 7; i++) {
            ps2GetData();
        }
        
        keyInput.cmd = PAUSE;
        keyInput.pressedDown = true;
        return keyInput;
    }

    if (scancode == 0xE0) {
        additionalCmd = true;
        scancode = ps2GetData();

        if (scancode == 0x12) {
            // Print screen
            ps2GetData();
            ps2GetData();

            keyInput.cmd = PRINT_SCREEN;
            keyInput.pressedDown = true;

            return keyInput;
        }

        if (scancode == 0x4A) {
            // Keypad '/'
            keyInput.cmd = NOT_CMD;
            keyInput.data = '/';
            keyInput.pressedDown = true;

            return keyInput;
        }

        if (scancode == 0x5A) {
            // Keypad '\n'
            keyInput.cmd = NOT_CMD;
            keyInput.data = '\n';
            keyInput.pressedDown = true;

            return keyInput;
        }
        
        if (scancode != 0xF0) {
            // Must be a additional cmd press
            keyInput.cmd = scancode;
            keyInput.pressedDown = true;

            return keyInput;
        }
    }

    if (scancode == 0xF0) {
        scancode = ps2GetData();

        if (additionalCmd) {
            if (scancode == 0x7C) {
                // Must be print screen release
                for (int i = 0; i < 3; i++) {
                    ps2GetData();
                }

                keyInput.cmd = PRINT_SCREEN;
                keyInput.pressedDown = false;

                return keyInput;
            }

            if (scancode == 0x4A) {
                // Keypad '/'
                keyInput.cmd = NOT_CMD;
                keyInput.data = '/';
                keyInput.pressedDown = false;

                return keyInput;
            }

            if (scancode == 0x5A) {
                // Keypad '\n'
                keyInput.cmd = NOT_CMD;
                keyInput.data = '\n';
                keyInput.pressedDown = false;

                return keyInput;
            }

            // Release of additional command key
            keyInput.cmd = scancode;
            keyInput.pressedDown = false;

            return keyInput;
        }

        keyInput.pressedDown = false;
    }

    // Uses a main key / command
    if (_scancode_set_2[scancode] == 0) {
        keyInput.cmd = scancode;

        // Update static key presses (shift, caps, nums locks, ...)
        switch (scancode) {
            case CAPS_LOCK:
                if (keyInput.pressedDown)
                    _caps_lock = !_caps_lock;

                break;
            case LEFT_SHIFT:
            case RIGHT_SHIFT:
                _shift_press = keyInput.pressedDown;
                break;
            case NUMBER_LOCK:
                if (keyInput.pressedDown)
                    _number_lock = !_number_lock;

                break;
        }
    } else {
        keyInput.cmd = NOT_CMD;
        keyInput.data = _scancode_set_2[scancode];

        // Check upper/lowercase of scancode
        if (isLetter(keyInput.data) && ((_caps_lock ^ _shift_press) == 1)) {
            keyInput.data = _lower_to_upper_char(keyInput.data);
        } else if (_shift_press) {
            keyInput.data = _lower_to_upper_char(keyInput.data);
        }
    }

    return keyInput;
}

key_st irqGetKeyboardChar() {
    key_st keyInput = {true, 0, UNSUPPORTED_CMD};

    if (_current_scancode_set == 2) {
        return irqKeyboard2GetChar();
    } else if (_current_scancode_set == 1) {
        return irqKeyboard1GetChar();
    } else {
        return keyInput;
    }
}

void ps2SetFlush(bool newFlush) {
    _flush_enabled = newFlush;
}

bool ps2GetFlush() {
    return _flush_enabled;
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
