#include "MPU6050.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <math.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

#include "esp_log.h"

#include "driver/i2c_master.h"

#include "config_i2c.h"
#include "i2c_device.h"

//-----------------------------------
// не менять последовательность
#include "i2c_device_private.h"
#include "i2c_device.h"
//-----------------------------------

//----------------------------------------------------------------------
#define TAG "MPU6050"
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// { "cmd": "add_sensor", "type": "MPU6050", "addr": 104 }
//----------------------------------------------------------------------
static esp_err_t MPU6050_init(i2c_device_t *device)
{
    ESP_LOGI(TAG, "MPU init at 0x%02X", device->dev_cfg.device_address);

    return ESP_OK;
}
//----------------------------------------------------------------------
static esp_err_t MPU6050_read(i2c_device_t *device, void *data)
{
    // float *f = (float *)data;

    // // fake data
    // f[0] = 1.0f;
    // f[1] = 2.0f;
    // f[2] = 3.0f;

    // static float t = 0;
    // t += 0.1f;

    // out->x = sinf(t);
    // out->y = cosf(t);
    // out->z = sinf(t * 0.5f);

    return ESP_OK;
}
//----------------------------------------------------------------------
// const i2c_sensor_ops_t MPU6050_ops = {
//     .name = "MPU6050",
//     .init = MPU6050_init,
//     .read = MPU6050_read};
//----------------------------------------------------------------------
// Создание backend - структуры с персонализированными функциями драйвера
const i2c_device_backend_t _MPU6050_backend = {
    MPU6050_init,
    MPU6050_read};

//----------------------------------------------------------------------
esp_err_t MPU6050_setup(i2c_device_t *device)
{
    ESP_LOGI(TAG, "start setup");

    device->backend = &_MPU6050_backend;

    return ESP_OK;
}