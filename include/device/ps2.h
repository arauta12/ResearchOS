#ifndef PS2_H
#define PS2_H

#include <io.h>
#include <stdbool.h>
#include <stdint.h>

// IO ports
#define PS2_DATA_PORT   0x60 /* reading Output & write input buffers */
#define PS2_CMD_PORT    0x64 /* read status reg & write cmd register */

// PS2 Commands
#define PS2_READ_CONFIG_BYTE            0x20 /* resp w/ the byte */
#define PS2_WRITE_CONFIG_BYTE           0x60
#define PS2_DISABLE_PORT_2              0xA7
#define PS2_ENABLE_PORT_2               0xA8
#define PS2_TEST_PORT_2                 0xA9 /* resp: 0 for pass, other for fail */
#define PS2_TEST_CONTROLLER             0xAA /* 0xFF for pass, 0xFC for fail */
#define PS2_TEST_PORT_1                 0xAB /* resp: same as for PORT 2 */
#define PS2_DIAGNOSTIC_DUMP             0xAC
#define PS2_DISABLE_PORT_1              0xAD
#define PS2_ENABLE_PORT_1               0xAE
#define PS2_READ_OUTPUT_PORT            0xD0 /* Reading Controller Output port, resp: the byte */
#define PS2_WRITE_OUTPUT_PORT           0xD1 /* Write next byte to the Output port */
#define PS2_WRITE_PORT_1_OUTPUT_BUFFER  0xD2 /* Appears like Port 1 sent next byte to us */
#define PS2_WRITE_PORT_2_OUTPUT_BUFFER  0xD3 /* Appears like Port 2 sent next byte to us */
#define PS2_WRITE_PORT_2_INPUT_BUFFER   0xD4 /* Tell PS/2 to send byte to Port 2 */
#define PS2_DEVICE_RESET                0xFF

static bool _wait_input_buf_clear();
static bool _wait_input_buf_set();
static bool _wait_output_buf_clear();
static bool _wait_output_buf_set();

static uint8_t _read_status_register();
static uint8_t _read_config_byte();
static uint8_t _read_output_port();

static void _write_config_byte(uint8_t);
static void _write_output_port(uint8_t);

static bool _is_valid_output_port(uint8_t);
static bool _is_valid_config_byte(uint8_t);

void ps2TogglePort(int, bool);
bool ps2TestPort(int);
bool ps2TestController();
void ps2SendCommand(uint8_t);
uint8_t ps2GetData();
bool ps2SendPort1Data(uint8_t);
bool ps2SendPort2Data(uint8_t);
bool ps2ToggleTranslation(bool);
bool ps2ResetPort(int);
const bool* getPs2DeviceStatusList(); 

bool ps2Initiate();

#endif
