#ifndef I2C_DEVICE_PRIVATE_H
#define I2C_DEVICE_PRIVATE_H
//----------------------------------------------------------------------
#include "i2c_device.h"

// #include "esp_log.h"
// #include "esp_err.h"

#include "driver/i2c_master.h"

//----------------------------------------------------------------------
typedef struct _i2c_device_backend
{
    esp_err_t (*init)(i2c_device_t *device);
    esp_err_t (*read)(i2c_device_t *device, void *data);
} i2c_device_backend_t;
//----------------------------------------------------------------------
struct _i2c_device
{
    i2c_device_config_t dev_cfg;
    const char *type_name;

    i2c_master_dev_handle_t dev;

    const i2c_device_backend_t *backend;
    // void *ctx;
};
//----------------------------------------------------------------------
#endif // I2C_DEVICE_PRIVATE_H