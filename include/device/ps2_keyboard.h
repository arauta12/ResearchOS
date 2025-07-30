#ifndef PS2_KEYBOARD_H
#define PS2_KEYBOARD_H

#include <io.h>
#include <stdint.h>
#include <stdbool.h>

/*
https://wiki.osdev.org/PS/2_Keyboard
*/

// Commands
#define PS2_KEYBOARD_ECHO           0xEE
#define PS2_KEYBOARD_SCANCODE       0xF0
#define PS2_KEYBOARD_IDENTIFY       0xF2
#define PS2_KEYBOARD_TYPE_OPT       0xF3
#define PS2_KEYBOARD_ENABLE         0xF4
#define PS2_KEYBOARD_DISABLE        0xF5
#define PS2_KEYBOARD_DEFAULTS       0xF6
#define PS2_KEYBOARD_RESEND_BYTE    0xFE
#define PS2_KEYBOARD_RESET          0xFF

#define PS2_KEYBOARD_ERROR_1        0x00
#define PS2_KEYBOARD_PASS_TEST      0xAA
#define PS2_KEYBOARD_ECHO_RESP      0xEE
#define PS2_KEYBOARD_CMD_ACK        0xFA
#define PS2_KEYBOARD_TEST_FAIL_1    0xFC
#define PS2_KEYBOARD_TEST_FAIL_2    0xFD
#define PS2_KEYBOARD_ERROR_2        0xFF

#define PS2_MAX_CODE_LEN            8

static uint8_t _scancode_set_1[] = {
    0,    0,   '1',  '2',  '3',  '4',  '5',  '6',  // 0x00–0x07
   '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t', // 0x08–0x0F
   'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  // 0x10–0x17
   'o',  'p',  '[',  ']',  '\n',  0,    'a',  's',  // 0x18–0x1F
   'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  // 0x20–0x27
   '\'', '`',   0,  '\\',  'z',  'x',  'c',  'v',   // 0x28–0x2F
   'b',  'n',  'm',  ',',  '.',  '/',   0,   '*',  // 0x30–0x37
    0,   ' ',   0,    0,    0,    0,    0,    0,    // 0x38–0x3F
    0,    0,    0,    0,    0,    0,    0,   '7', // 0x40–0x47
   '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',  // 0x48–0x4F
   '2',  '3',  '0',  '.',   0,    0,    0,    0     // 0x50–0x57
};

static uint8_t _scancode_set_2[] = {
   0,    0, 0, 0, 0, 0, 0, 0, // 0x00-0x07
   0,    0, 0, 0, 0, '\t', '`', 0,   // 0x08-0x0F
   0,    0, 0, 0, 0, 'q', '1', 0,    // 0x10-0x17
   0,    0, 'z', 's', 'a', 'w', '2', 0, // 0x18-0x1F
   0,   'c', 'x', 'd', 'e', '4', '3', 9,  // 0x20-0x27
   0,   ' ', 'v', 'f', 't', 'r', '5', 0,  // 0x28-0x2F
   0,   'n', 'b', 'h', 'g', 'y', '6', 0,  // 0x30-0x37
   0,    0, 'm', 'j', 'u', '7', '8', 0,    // 0x38-0x3F
   0,   ',', 'k', 'i', 'o', '0', '9', 0,     // 0x40-0x47
   0,   '.', '/', 'l', ';', 'p', '-', 0,     // 0x48-0x4F
   0,    0, '\'', 0, '[', '=', 0, 0,         // 0x50-0x57
   0,    0, '\n', ']', 0, '\\', 0, 0,           // 0x58-0x5F
   0,    0, 0, 0, 0, 0, '\b', 0,           // 0x60-0x67
   0,   '1', 0, '4', '7', 0, 0, 0,       // 0x68-0x6F
   '0', '.', '2', '5', '6', '8', 0, 0, // 0x70-0x77
    0,  '+', '3', '-', '*', '9', 0, 0,   // 0x78-0x7F
};

static int _current_scancode_set;
static bool _caps_lock;
static bool _shift_press;

typedef enum {
    NOT_CMD = 0,
    UNSUPPORTED_CMD,
    ESCAPE,
    HOME,
    END,
    INSERT,
    DELETE,
    POWER,
    SLEEP,
    WAKE,
    NUMBER_LOCK,
    CAPS_LOCK_ON,
    CAPS_LOCK_OFF,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    LEFT_CTRL,
    RIGHT_CTRL,
    LEFT_ALT,
    RIGHT_ALT,
    CURSOR_LEFT,
    CURSOR_RIGHT,
    CURSOR_UP,
    CURSOR_DOWN,
    PAGE_UP,
    PAGE_DOWN,
} KEY_CMD;

typedef struct {
    bool pressedDown;
    uint8_t data;
    KEY_CMD cmd;
} key_st;

typedef struct {
    bool pressedDown;
    uint8_t letter;
} KEYCHAR;

static uint8_t _lower_to_upper_char(uint8_t);

void flushKeyBuffer();
bool ps2KeyboardEcho();
uint8_t ps2KeyboardGetScancodeSet();
bool ps2KeyboardSetScancodeSet(uint8_t);
bool ps2KeyboardSetTypeRate(uint8_t);
bool ps2KeyboardToggleScan(bool);
bool ps2KeyboardSetDefault();
bool ps2KeyboardSelfTest();

KEYCHAR irqKeyboard1GetKey();
KEYCHAR irqKeyboard2GetChar();
KEYCHAR irqGetKeyboardChar();

bool ps2KeyboardConfig();

#endif
