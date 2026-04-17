#include "msp.h"

#include "bluetooth.h"
#include "ble_data.h"
//----------------------------------------------------------------------
static void msp_send(uint8_t cmd, uint8_t *data, uint8_t size)
{
    uint8_t buf[256];
    uint8_t i = 0;
    uint8_t checksum = 0;

    buf[i++] = '$';
    buf[i++] = 'M';
    buf[i++] = '>';

    buf[i++] = size;
    checksum ^= size;

    buf[i++] = cmd;
    checksum ^= cmd;

    for (int j = 0; j < size; j++)
    {
        buf[i++] = data[j];
        checksum ^= data[j];
    }

    buf[i++] = checksum;

    ble_send(buf, i); // 👉 отправка через BLE
}
//----------------------------------------------------------------------
void msp_send_attitude(int16_t roll, int16_t pitch, int16_t yaw)
{
    uint8_t data[6];

    data[0] = roll & 0xFF;
    data[1] = roll >> 8;

    data[2] = pitch & 0xFF;
    data[3] = pitch >> 8;

    data[4] = yaw & 0xFF;
    data[5] = yaw >> 8;

    msp_send(MSP_ATTITUDE, data, 6);
}
//----------------------------------------------------------------------