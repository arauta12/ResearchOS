#include <arch/i386/ps2.h>
#include <early_kprintf.h>

static bool ps2DeviceActive[2] = {false, false};
static const int MAX_ATTEMPTS = 5000;

/*
    Get byte from PS2 status register
    Bit 0: output buffer is empty (0) / full (1)
    Bit 1: input buffer is empty (0) / full (1)
    (Bit 2: sys flag for tests (don't touch))
    Bit 3: written input byte is for: device (0) / cmd (1)
    Bit 4: ?
    Bit 5: ?
    Bit 6: timeout: no error (0), error (1)
    Bit 7: parity: no error (0), error (1)
*/
static uint8_t _read_status_register() { return inb(PS2_CMD_PORT); }

/*
    Stall until input buffer is empty
    If it takes too long, returns false
*/
static bool _wait_input_buf_clear() {
    bool input_clear = false;
    int attempts = 0;

    do {
        uint8_t status = _read_status_register();
        input_clear = (status == (status & 0xfd));
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
    bool input_set = false;
    int attempts = 0;

    do {
        uint8_t status = _read_status_register();
        input_set = (status == (status | 0x02));
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
    bool output_clear = false;
    int attempts = 0;

    do {
        uint8_t status = _read_status_register();
        output_clear = (status % 2 == 0);
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
    bool output_set = false;
    int attempts = 0;

    do {
        uint8_t status = _read_status_register();
        output_set = (status % 2 == 1);
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
    if (!_wait_output_buf_set()) return 0xff;  // return a garbage config byte

    return inb(PS2_DATA_PORT);
}

/*
    Attempts to set config byte
    NOTE: does not set the byte if the wait is too long!
    - Try to check if it is set.
*/
static void _write_config_byte(uint8_t config) {
    outb(PS2_CMD_PORT, PS2_WRITE_CONFIG_BYTE);
    if (!_wait_input_buf_clear()) return;

    outb(PS2_DATA_PORT, config);
    io_wait();
}

/*
    Read controller output port
    If unable to, returns 0xff
    NOTE: check this before using it (bit 0 should be 1!)
*/
static uint8_t _read_output_port() {
    outb(PS2_CMD_PORT, PS2_READ_OUTPUT_PORT);
    if (!_wait_output_buf_set()) return 0xff;

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
    if (!_wait_input_buf_clear()) return;

    outb(PS2_DATA_PORT, data);
    io_wait();
}

/*
    Return true if controller output port is valid, false otherwise
    NOTE: Only checks if bit 0 is set to 1 (System reset bit)
 */
static bool _is_valid_output_port(uint8_t data) { return (data % 2 == 1); }

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
void ps2_toggle_port(int portNum, bool enable) {
    if (portNum < 1 || portNum > 2) return;

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
bool ps2_test_port(int portNum) {
    if (portNum < 1 || portNum > 2) return false;

    if (portNum == 1) {
        outb(PS2_CMD_PORT, PS2_TEST_PORT_1);
    } else {
        outb(PS2_CMD_PORT, PS2_TEST_PORT_2);
    }

    if (!_wait_output_buf_set()) return false;

    uint8_t resp = inb(PS2_DATA_PORT);
    io_wait();

    return (resp == 0x00);
}

/*
    Does a test of the PS/2 Controller
    Returns false if waits too long or does not return
        success byte
*/
bool ps2_test_controller() {
    outb(PS2_CMD_PORT, PS2_TEST_CONTROLLER);
    if (!_wait_output_buf_set()) return false;

    uint8_t resp = inb(PS2_DATA_PORT);
    io_wait();

    return (resp == 0x55);
}

void ps2_send_command(uint8_t cmd) {
    outb(PS2_CMD_PORT, cmd);
    io_wait();
}

uint8_t ps2_get_data() {
    if (!_wait_output_buf_set()) return 0x00;

    uint8_t resp = inb(PS2_DATA_PORT);
    io_wait();
    return resp;
}

/**
 * Sends data to 1st PS2 Port and check acknowledgment
 * @param data Byte of data to send to the port
 * @return True if successful, false if it failed
 */
bool ps2_send_port1_data_ack(uint8_t data) {
    if (!_wait_input_buf_clear()) return false;

    outb(PS2_DATA_PORT, data);
    if (!_wait_output_buf_set()) return false;

    return (inb(PS2_DATA_PORT) == PS2_PORT_1_CMD_ACK);
}

bool ps2_send_port2_data_ack(uint8_t data) {
    outb(PS2_CMD_PORT, PS2_WRITE_PORT_2_INPUT_BUFFER);

    if (!_wait_input_buf_clear()) {
        kerror("PS2 Port 2 failed to clear input buffer.\n");
        return false;
    }

    outb(PS2_DATA_PORT, data);
    io_wait();

    if (!_wait_output_buf_set()) {
        kerror("PS2 Port 2 failed to clear input buffer.\n");
        return false;
    }

    return (inb(PS2_DATA_PORT) == 0xFA);
}

bool ps2_send_port1_data(uint8_t data) {
    if (!_wait_input_buf_clear()) return false;

    outb(PS2_DATA_PORT, data);
    if (!_wait_output_buf_set()) return false;

    return true;
}

bool ps2_send_port2_data(uint8_t data) {
    outb(PS2_CMD_PORT, PS2_WRITE_PORT_2_INPUT_BUFFER);

    if (!_wait_input_buf_clear()) {
        kerror("PS2 Port 2 failed to clear input buffer.\n");
        return false;
    }

    outb(PS2_DATA_PORT, data);
    io_wait();

    if (!_wait_output_buf_set()) {
        kerror("PS2 Port 2 failed to clear input buffer.\n");
        return false;
    }

    return true;
}

bool ps2_toggle_translation(bool enable) {
    uint8_t currentConfig = _read_config_byte();
    if (!_is_valid_config_byte(currentConfig)) return false;

    uint8_t newConfig = currentConfig;
    if (enable) {
        newConfig |= 0x40;
    } else {
        newConfig &= 0x37;
    }

    _write_config_byte(newConfig);
    return true;
}

const bool* get_ps2_device_status_list() { return ps2DeviceActive; }

bool ps2_reset_port(int portNum) {
    if (portNum < 1 || portNum > 3) return false;

    // Send data
    if (portNum == 1 && !ps2_send_port1_data(PS2_DEVICE_RESET)) {
        kerror("Failed to send data to PS2 Port 1!\n");
        return false;
    } else if (portNum == 2 && !ps2_send_port2_data(PS2_DEVICE_RESET)) {
        kerror("Failed to send data to PS2 Port 2!\n");
        return false;
    }

    if (!_wait_output_buf_set()) return false;

    // Check 1st response
    uint8_t resp = inb(PS2_DATA_PORT);
    io_wait();
    if (resp != 0xAA && resp != 0xFA) {
        kerror("Reset failed for PS2 Port %d: resp=%x!\n", portNum, resp);
        return false;
    }

    if (!_wait_output_buf_set()) return false;

    // Check 2nd response
    resp = inb(PS2_DATA_PORT);
    io_wait();
    if (resp != 0xAA && resp != 0xFA) {
        kerror("Reset failed for PS2 Port %d: resp=%x!\n", portNum, resp);
        return false;
    }

    // Receive Port 2 (mouse) ID
    if (portNum == 2) {
        if (!_wait_output_buf_set()) return false;

        resp = inb(PS2_DATA_PORT);
        io_wait();
    }

    return true;
}

bool ps2_initiate() {
    kprintf("Initiating PS2 config...\n");

    // disable devices (don't send data)
    ps2_toggle_port(1, false);
    ps2_toggle_port(2, false);

    // flush output buffer
    inb(PS2_DATA_PORT);
    io_wait();

    // set config byte
    uint8_t oldConfig = _read_config_byte();
    if (!_is_valid_config_byte(oldConfig)) {
        kerror("PS2 config: Can't set config!\n");
        return false;
    }

    _write_config_byte(oldConfig & 0x26);

    // test controller
    if (!ps2_test_controller()) {
        kerror("PS2 config: Can't test controller!\n");
        return false;
    }

    // 2 channel test
    ps2_toggle_port(2, true);
    uint8_t config = _read_config_byte();
    if (config == (config & 0xdf)) {
        ps2_toggle_port(2, false);
        _write_config_byte(config & 0x15);
        ps2DeviceActive[1] = true;
    }

    // test ports
    if (!ps2_test_port(1)) {
        kerror("PS2 config: Test Port 1 failed!\n");
        ps2DeviceActive[0] = false;
    } else {
        ps2DeviceActive[0] = true;
    }

    if (ps2DeviceActive[1]) {
        if (!ps2_test_port(2)) {
            kerror("PS2 config: Test Port 2 failed!\n");
            ps2DeviceActive[1] = false;
        }
    }

    if (!ps2DeviceActive[0] && !ps2DeviceActive[1]) {
        kerror("PS2 config: both ports failed!\n");
        return false;
    }

    // Enable devices
    config = _read_config_byte();
    if (!_is_valid_config_byte(config)) {
        kerror("PS2 config: failed to enable Port 1!\n");
        ps2DeviceActive[0] = false;
        ps2DeviceActive[1] = false;
        return false;
    }

    if (ps2DeviceActive[0]) {
        ps2_toggle_port(1, true);
        config |= 0x1;
    }

    if (ps2DeviceActive[1]) {
        ps2_toggle_port(2, true);
        config |= 0x2;
    }

    _write_config_byte(config);

    // Reset devices
    bool resetSuccess = ps2_reset_port(1);
    if (!resetSuccess) {
        ps2DeviceActive[0] = false;
        kerror("PS2 config: Port 1 reset issue!\n");
    }

    resetSuccess = ps2_reset_port(2);
    if (!resetSuccess) {
        ps2DeviceActive[1] = false;
        kerror("PS2 config: Port 2 reset issue!\n");
    }

    if (!ps2DeviceActive[0] && !ps2DeviceActive[1]) {
        kerror("PS2 config: both ports failed!\n");
        return false;
    }

    kprintf("PS2 config done!\n");
    kprintf("PS2 devices? Dev1: %d, Dev2: %d\n", ps2DeviceActive[0],
            ps2DeviceActive[1]);
    return true;
}