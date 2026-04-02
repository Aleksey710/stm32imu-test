#include "i2c_devices.h"

#include "esp_log.h"
#include "esp_err.h"

#include "driver/i2c_master.h"

#include "config_i2c.h"
#include "i2c.h"

//-----------------------------------
// не менять последовательность
#include "i2c_device_private.h"
#include "i2c_device.h"
//-----------------------------------

#include "MPU6050.h"
//----------------------------------------------------------------------
#define TAG "I2C SETUP DEVICES"
//----------------------------------------------------------------------
void i2c_device_init(i2c_device_t *device)
{
    uint8_t addr = device->dev_cfg.device_address;

    switch (addr)
    {
    case DEVICE_ADDRESS::MPU6050_LOW:
    case DEVICE_ADDRESS::MPU6050_HIGHT:
    {
        // s->ops = &MPU6050_ops;
        // s->ctx = calloc(1, sizeof(MPU6050_ctx_t));
        if (MPU6050_setup(device) != ESP_OK)
            return;
    }
    break;
    case 0x70:
    {
    }
    break;
    default:
    {
        ESP_LOGW(TAG, "Unknown device at 0x%02X", addr);
        i2c_master_bus_rm_device(device->dev);
        free(device);
        return;
    }
    break;
    }
}
//----------------------------------------------------------------------
