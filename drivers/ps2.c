#include <device/ps2.h>
#include <stdio.h>

/*
# PS/2 (I8042) Controller

## 1 - Links
- [Osdev site](https://wiki.osdev.org/I8042_PS/2_Controller)
- [Old KBC datasheet](https://web.archive.org/web/20210417040153/http://www.diakom.ru/el/elfirms/datashts/Smsc/42w11.pdf)
## 2 - General
Also known as the keyboard controller but can also connect other PS/2 devices.

Used to control just a keyboard, but expanded to include others, such a keyboard AND mouse.

As keyboards created new scancode sets, the PS/2 came with a translation option to scan set 2 into set 1 (and apparently enabled by default).

## 3 - Details
### Output and Input buffers (from Device POV)
There are two buffers of 1 byte for data: output and input buffer (from the perspective of the DEVICE not OS). Devices send data to the output buffer for the OS to receive and receive data from the OS through the input buffer.

### IO Ports
There are usually two IO ports: 0x60 and 0x64. The 0x60 (data) port is used to read/write from the buffers mentioned above.

In addition to the buffers, there are two internal registers: status & command register:
- **Status** (reading from 0x64): PS/2 flags indicating state of controller (& its buffers), such as:
  - Full/empty output buffer, full/empty input buffer, system flag, where input buffer hold a command or data, timeouts, parity errors
- **Command** (writing to 0x64): telling the PS/2 controller itself to do something, ie. disable/enable a port, testing, etc.

### Controller configuration byte
The PS/2 controller has a special name for byte 0 of its internal RAM (which of course it has one): Controller configuration byte. This can be read and set with certain PS/2 commands, which involve things like:
- Enabling/disabling port 1 & 2 interrupts
- Enabling/disabling port 1 & 2 clocks
- Enabling/disabling translation (mentioned at the beginning)

### Controller Output Port
This is a byte that controls things such as:
- System reset (which should NOT be touched)
- The A20 gate
- 2nd port clock and data
- Which device filled the output buffer
- 1st ps2 port clock and data
This is able to be read and written with the 0xD0 and 0xD1 commands.
*/

static bool ps2DeviceActive[2] = {false, false};

/* Get byte from PS2 status register */
static uint8_t _read_status_register() {
    return inb(PS2_CMD_PORT);
}

/* 
    Stall until input buffer is empty
    If it takes too long, returns false
*/
static bool _wait_input_buf_clear() {
    const int MAX_ATTEMPTS = 1000;
    bool input_clear = false;
    int attempts = 0;

    do {
        uint8_t status = _read_status_register();
        input_clear = ( status == (status & 0xfd) );
        attempts++;
        io_wait();
    } while (attempts < MAX_ATTEMPTS && !input_clear);

    return input_clear;
}

/*
    Stall until input buffer is set
    If it takes too long, returns false
*/
static bool _wait_input_buf_set() {
    const int MAX_ATTEMPTS = 1000;
    bool input_set = false;
    int attempts = 0;

    do {
        uint8_t status = _read_status_register();
        input_set = ( status == (status | 0x02) );
        attempts++;
        io_wait();
    } while (attempts < MAX_ATTEMPTS && !input_set);

    return input_set;
}

/* 
    Stall until output buffer is empty
    If it takes too long, returns false
*/
static bool _wait_output_buf_clear() {
    const int MAX_ATTEMPTS = 1000;
    bool output_clear = false;
    int attempts = 0;

    do {
        uint8_t status = _read_status_register();
        output_clear = ( status % 2 == 0 );
        attempts++;
        io_wait();
    } while (attempts < MAX_ATTEMPTS && !output_clear);

    return output_clear;
}

/* 
    Stall until output buffer is set
    If it takes too long, returns false
*/
static bool _wait_output_buf_set() {
    const int MAX_ATTEMPTS = 1000;
    bool output_set = false;
    int attempts = 0;

    do {
        uint8_t status = _read_status_register();
        output_set = ( status % 2 == 1 );
        attempts++;
        io_wait();
    } while (attempts < MAX_ATTEMPTS && !output_set);

    return output_set;
}

/* 
    Read configuration byte (byte 0 of internal RAM) 
    If unable to, return 0xff, which is invalid (bit 3,7 should be 0)
*/
static uint8_t _read_config_byte() {
    outb(PS2_CMD_PORT, PS2_READ_CONFIG_BYTE);
    if(!_wait_output_buf_set())
        return 0xff; // return a garbage config byte

    return inb(PS2_DATA_PORT);

}

/* 
    Attempts to set config byte
    NOTE: does not set the byte if the wait is too long!
    - Try to check if it is set.
*/
static void _write_config_byte(uint8_t config) {
    outb(PS2_CMD_PORT, PS2_WRITE_CONFIG_BYTE);
    if(!_wait_input_buf_clear())
        return;

    outb(PS2_DATA_PORT, config);
    io_wait();
}

/* 
    Read controller output port 
    If unable to, returns 0x0
    NOTE: check this before using it (bit 0 should be 1!)
*/
static uint8_t _read_output_port() {
    outb(PS2_CMD_PORT, PS2_READ_OUTPUT_PORT);
    if(!_wait_output_buf_set())
        return 0x0;

    return inb(PS2_DATA_PORT);
}

/*
    Attempts to set output port byte to given one
    NOTE: Not guaranteed to happen if delay is too long
    - Please check if it is actually written
    - No check is done on data, it is up to you to check its validity
*/
static void _write_output_port(uint8_t data) {
    outb(PS2_CMD_PORT, PS2_WRITE_OUTPUT_PORT);
    if(!_wait_input_buf_clear())
        return;

    outb(PS2_DATA_PORT, data);
}

/* 
    Return true if controller output port is valid, false otherwise
    NOTE: Only checks if bit 0 is set to 1 (System reset bit)
 */
static bool _is_valid_output_port(uint8_t data) {
    return (data % 2 == 1);
}

/*
    Return true if controller configuration byte is valid, false otherwise
    NOTE: only check if bits 3 & 7 are 0
*/
static bool _is_valid_config_byte(uint8_t config) {
    return (config == (config & 0x77));
}

/*
    Enables/disables the PS/2 port specified
    NOTE: must either be 1 or 2 (for now) 
*/
void ps2TogglePort(int portNum, bool enable) {
    if (portNum < 1 || portNum > 2)
        return;

    if (portNum == 1) {
        outb(PS2_CMD_PORT, PS2_DISABLE_PORT_1 + enable);
        io_wait();
    } else {
        outb(PS2_CMD_PORT, PS2_DISABLE_PORT_2 + enable);
        io_wait();
    }
}

/*
    Does a test of the PS/2 port specified
    Returns false if waits too long or does not return
        success byte
    NOTE: must either be 1 or 2 (for now) 
*/
bool ps2TestPort(int portNum) {
    if (portNum < 1 || portNum > 2)
        return false;

    if (portNum == 1) {
        outb(PS2_CMD_PORT, PS2_TEST_PORT_1);
    } else {
        outb(PS2_CMD_PORT, PS2_TEST_PORT_2);
    }

    if(!_wait_output_buf_set())
        return false;

    uint8_t resp = inb(PS2_DATA_PORT);
    io_wait();

    return (resp == 0x00);
}

/*
    Does a test of the PS/2 Controller
    Returns false if waits too long or does not return
        success byte
*/
bool ps2TestController() {
    outb(PS2_CMD_PORT, PS2_TEST_CONTROLLER);
    if(!_wait_output_buf_set())
        return false;

    uint8_t resp = inb(PS2_DATA_PORT);
    io_wait();

    return (resp == 0x55);
}

void ps2SendCommand(uint8_t cmd) {
    outb(PS2_CMD_PORT, cmd);
    io_wait();
}

uint8_t ps2GetData() {
    uint8_t resp = inb(PS2_DATA_PORT);
    io_wait();
    return resp;
}

bool ps2SendPort1Data(uint8_t data) {
    if (!_wait_input_buf_clear())
        return false;

    outb(PS2_DATA_PORT, data);
    io_wait();

    return true;
}

bool ps2SendPort2Data(uint8_t data) {
    outb(PS2_CMD_PORT, PS2_WRITE_PORT_2_INPUT_BUFFER);

    if (!_wait_input_buf_clear()) {
        kprintf("PS2 Port 2 data send: failed to clear input buffer.\b");
        return false;
    }

    outb(PS2_DATA_PORT, data);
    io_wait();

    return true;
}

bool ps2ToggleTranslation(bool enable) {
    uint8_t currentConfig = _read_config_byte();
    if (!_is_valid_config_byte(currentConfig))
        return false;

    uint8_t newConfig = currentConfig;
    if (enable) {
        newConfig |= 0x40;
    } else {
        newConfig &= 0x37;
    }
    
    _write_config_byte(newConfig);
    return true;
}

const bool* getPs2DeviceStatusList() {
    return ps2DeviceActive;
}

bool ps2ResetPort(int portNum) {
    if (portNum < 1 || portNum > 3)
        return false;

    if (portNum == 1) {
        ps2SendPort1Data(PS2_DEVICE_RESET);
    } else {
        ps2SendPort2Data(PS2_DEVICE_RESET);
    }

    if (!_wait_output_buf_set())
        return false;

    uint8_t resp = inb(PS2_DATA_PORT);
    io_wait();
    if (resp != 0xFA && resp != 0xAA)
        return false;

    if (!_wait_output_buf_set())
        return false;

    resp = inb(PS2_DATA_PORT);
    if (resp != 0xFA && resp != 0xAA)
        return false;
    
    return true;
} 

bool ps2Initiate() {

    kprintf("Initiating PS2 config...\n");

    // disable devices (don't send data)
    ps2TogglePort(1, false);
    ps2TogglePort(2, false);

    // flush output buffer
    inb(PS2_DATA_PORT);
    io_wait();

    // set config byte
    uint8_t oldConfig = _read_config_byte();
    if (!_is_valid_config_byte(oldConfig)) {
        kprintf("(ERROR) PS2 config: Can't set config!\n");
        return false;
    }

    _write_config_byte(oldConfig & 0x26);

    // test controller
    if(!ps2TestController()) {
        kprintf("(ERROR) PS2 config: Can't test controller!\n");
        return false;
    }

    // 2 channel test
    ps2TogglePort(2, true);
    uint8_t config = _read_config_byte();
    if (config == (config & 0xdf)) {
        ps2TogglePort(2, false);
        _write_config_byte(config & 0x15);
        ps2DeviceActive[1] = true;
    }

    // test ports
    if (!ps2TestPort(1)) {
        kprintf("(ERROR) PS2 config: Test Port 1 failed!\n");
        ps2DeviceActive[0] = false;
    } else {
        ps2DeviceActive[0] = true;
    }

    if (ps2DeviceActive[1]) {
        if (!ps2TestPort(2)) {
            kprintf("(ERROR) PS2 config: Test Port 2 failed!\n");
            ps2DeviceActive[1] = false;
        }
    }

    if (!ps2DeviceActive[0] && !ps2DeviceActive[1]) {
        kprintf("(ERROR) PS2 config: both ports failed!\n");
        return false;
    }

    // Enable devices
    config = _read_config_byte();
    if (!_is_valid_config_byte(config)) {
        kprintf("(ERROR) PS2 config: failed to enable Port 1!\n");
        ps2DeviceActive[0] = false;
        ps2DeviceActive[1] = false;
        return false;
    } 

    if (ps2DeviceActive[0]) {
        ps2TogglePort(1, true);
        config |= 0x1;
    }

    if (ps2DeviceActive[1]) {
        ps2TogglePort(2, true);
        config |= 0x2;
    }

    _write_config_byte(config);


    // Reset devices
    bool resetSuccess = ps2ResetPort(1);
    if (!resetSuccess) {
        ps2DeviceActive[0] = false;
        kprintf("(ERROR) PS2 config: Port 1 reset issue!\n");
    }

    resetSuccess = ps2ResetPort(2);
    if (!resetSuccess) {
        ps2DeviceActive[1] = false;
        kprintf("(ERROR) PS2 config: Port 2 reset issue!\n");
    }

    if (!ps2DeviceActive[0] && !ps2DeviceActive[1]) {
        kprintf("(ERROR) PS2 config: both ports failed!\n");
        return false;
    }

    kprintf("PS2 config done!\n");
    kprintf("PS2 devices? Dev1: %d, Dev2: %d\n", ps2DeviceActive[0], ps2DeviceActive[1]);
    return true;
}

