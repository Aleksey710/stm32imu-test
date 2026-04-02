#ifndef I2C_DEVICE_PRIVATE_H
#define I2C_DEVICE_PRIVATE_H
//----------------------------------------------------------------------
#include "i2c_device.h"

// #include "esp_log.h"
// #include "esp_err.h"

#include "driver/i2c_master.h"

//----------------------------------------------------------------------
// Бэкенд устройства - набор функций для работы с устройством
// i2c_device_backend_t - тип бэкенда
typedef struct _i2c_device_backend
{
    esp_err_t (*init)(i2c_device_t *device);
    esp_err_t (*read)(i2c_device_t *device, void *data);
} i2c_device_backend_t;
//----------------------------------------------------------------------
// Абстрактная структура устройства
struct _i2c_device
{
    const char *type_name;
    i2c_device_config_t dev_cfg;
    i2c_master_dev_handle_t dev;

    i2c_device_backend_t *backend;

    void *ctx;
};
//----------------------------------------------------------------------
#endif // I2C_DEVICE_PRIVATE_H