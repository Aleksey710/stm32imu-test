#ifndef I2C_DEVICES_H
#define I2C_DEVICES_H
//----------------------------------------------------------------------
#include <inttypes.h>
#include "i2c_device.h"
//----------------------------------------------------------------------
enum I2C_DEV_ADDR
{
    I2C_DEV_ADDR_DEVICE_NONE = 0x00,
    // ...
    I2C_DEV_ADDR_DEVICE_1 = 0x01,
    // ...
    I2C_DEV_ADDR_MPU6050_LOW = 0x68,
    I2C_DEV_ADDR_MPU6050_HIGHT = 0x69,
    // ...
    I2C_DEV_ADDR_DEVICE_LAST = 0xf0
};
//----------------------------------------------------------------------
void i2c_device_init(i2c_device_t *device);
//----------------------------------------------------------------------
#endif // I2C_DEVICES_H
