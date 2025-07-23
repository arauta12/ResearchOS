#ifndef PS2_KEYBOARD_H
#define PS2_KEYBOARD_H

#include <io.h>
#include <stdint.h>
#include <stdbool.h>

/*
https://wiki.osdev.org/PS/2_Keyboard
*/

#define PS2_KEYBOARD_ERROR          0xEE    // Key / buffer error
#define PS2_KEYBOARD_TEST_PASS      0xAA    // Keyboard sent for self test passed
#define PS2_KEYBOARD_ECHO           0xEE    // For diagnostics

#define PS2_KEYBOARD_SCANCODE_SET   0xF0    // Scancode command
#define PS2_KEYBOARD_CURRENT_SET    0x00    // Get current scancode
#define PS2_KEYBOARD_SET_1          0x01    // Scancode set 1
#define PS2_KEYBOARD_SET_2          0x02    // Scancode set 2
#define PS2_KEYBOARD_SET_3          0x03    // Scancode set 3

#define PS2_KEYBOARD_IDENTIFY       0xF2    // Detect device type

#define PS2_KEYBOARD_RATE_AND_DELAY 0xF3    // setting repeat rate & delay
#define PS2_KEYBOARD_30_HZ_RATE     0x0     // 30 Hz repeat rate
#define PS2_KEYBOARD_2_HZ_RATE      0x1f    // 2 Hz repeat rate
#define PS2_KEYBOARD_DELAY_250MS    0 << 5  // 250ms key delay for repeat  
#define PS2_KEYBOARD_DELAY_500MS    1 << 5  // 500ms key delay for repeat 
#define PS2_KEYBOARD_DELAY_750MS    2 << 5  // 750ms key delay for repeat 
#define PS2_KEYBOARD_DELAY_1000MS   3 << 5  // 1000ms key delay for repeat 

#define PS2_KEYBOARD_ENABLE         0xF4    // Clear buffer & begin scanning
#define PS2_KEYBOARD_DISABLE        0xF5    // Disable keyboard
#define PS2_KEYBOARD_DEFAULT        0xF6    // Use default
#define PS2_KEYBOARD_ACK            0xFA    // Command acknowledge
#define PS2_KEYBOARD_RESEND         0xFE    // Resend byte
#define PS2_KEYBOARD_RESET          0xFF    // Resetting keyboard       

// MISSING: escape, enter, CTRL, SHIFTS, ALTS, CAPS
// static uint8_t _scan_code_1_set[88]  = {
//     0, 0, '2', '3', '4', '5', '6', '7', '8', '9', '0',
//     '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y',
//         'u', 'i', 'o', 'p', '[', ']', 0,
//     0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
//     0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
//     0, ' ', 0, 0,
//     0, 0, 0, 0,
//     0, 0, 0, 0,
//     0, 0, 0, '7',
//     '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
//     0, 0, 0, 0
// };
static uint8_t _scan_code_1_set[] = { 
   0 , 0 , '1' , '2' , 
   '3' , '4' , '5' , '6' ,  
   '7' , '8' , '9' , '0' ,  
   '-' , '=' , 0 , 0 , 'Q' ,  
   'W' , 'E' , 'R' , 'T' , 'Y' , 
   'U' , 'I' , 'O' , 'P' , '[' , ']' ,  
   0 , 0 , 'A' , 'S' , 'D' , 'F' , 'G' ,  
   'H' , 'J' , 'K' , 'L' , ';' , '\'' , '`' ,  
   0 , '\\' , 'Z' , 'X' , 'C' , 'V' , 'B' , 'N' , 'M' , 
   ',' , '.' , '/' , 0 , '*' , 0 , ' ' 
};

static uint8_t _sc1_released_key_offset = 0x80;

uint8_t getScancode();
uint8_t getScancodeSet();
bool setScancodeSet(int setNum);
bool ps2KeyboardInit();

#endif
