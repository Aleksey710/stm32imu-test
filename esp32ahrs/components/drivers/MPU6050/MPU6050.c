#include "MPU6050.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// #include <math.h>
#include <mpu6050.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

#include "esp_log.h"

#include "driver/i2c_master.h"

#include "config.h"

#include "accelerometer_data.h"
#include "gyroscope_data.h"

#include "ws_msg_ringbuf.h"

//--------------------------------
// не менять последовательность //
#include "i2c_device_private.h" //
#include "i2c_device.h"         //
//--------------------------------
#include "i2c_devices.h"

//----------------------------------------------------------------------
#define TAG "MPU6050"
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// { "cmd": "add_sensor", "type": "MPU6050", "addr": 104 }
//----------------------------------------------------------------------
mpu6050_dev_t mpu6050_dev = {0};
//----------------------------------------------------------------------
static esp_err_t MPU6050_init(i2c_device_t *device)
{
    ESP_LOGI(TAG, "MPU init at 0x%02X", device->dev_cfg.device_address);

    ESP_ERROR_CHECK(mpu6050_init_desc(&mpu6050_dev,
                                      I2C_DEV_ADDR_MPU6050_LOW,
                                      0,
                                      I2C_SDA_GPIO,
                                      I2C_SCL_GPIO));

    while (1)
    {
        esp_err_t res = i2c_dev_probe(&mpu6050_dev.i2c_dev, I2C_DEV_WRITE);
        if (res == ESP_OK)
        {
            ESP_LOGI(TAG, "Found MPU60x0 device");
            break;
        }
        ESP_LOGE(TAG, "MPU60x0 not found");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_ERROR_CHECK(mpu6050_init(&mpu6050_dev));

    device->ctx = &mpu6050_dev;

    return ESP_OK;
}
//----------------------------------------------------------------------
static esp_err_t MPU6050_read(i2c_device_t *device, void *data)
{
    mpu6050_dev_t dev = *((mpu6050_dev_t *)(device->ctx));

    float temp;
    mpu6050_acceleration_t accel = {0};
    mpu6050_rotation_t rotation = {0};

    ESP_ERROR_CHECK(mpu6050_get_temperature(device, &temp));
    ESP_ERROR_CHECK(mpu6050_get_motion(device, &accel, &rotation));

    // ESP_LOGI(TAG, "**********************************************************************");
    // ESP_LOGI(TAG, "Acceleration: x=%.4f   y=%.4f   z=%.4f", accel.x, accel.y, accel.z);
    // ESP_LOGI(TAG, "Rotation:     x=%.4f   y=%.4f   z=%.4f", rotation.x, rotation.y, rotation.z);
    // ESP_LOGI(TAG, "Temperature:  %.1f", temp);
    /*
        mpu_data_t test_data;

        test_data.ax = accel.x;
        test_data.ay = accel.y;
        test_data.az = accel.z;

        test_data.gx = rotation.x;
        test_data.gy = rotation.y;
        test_data.gz = rotation.z;

        //--------------
        test_data.x = accel.x;
        test_data.y = accel.y;
        test_data.z = accel.z;

        // test_data.x = rotation.x;
        // test_data.y = rotation.y;
        // test_data.z = rotation.z;

        test_data.roll = 10.1f;
        test_data.pitch = 20.2f;
        test_data.yaw = 30.3f;
        //--------------
        // test_data.timestamp = 0;

        xQueueSend(ws_queue, &test_data, portMAX_DELAY);
        */
    //--------------------
    Ws_msg_t *msg = alloc_ws_msg();

    if (msg)
    {
        // snprintf(msg->data, WS_RINGBUFF_MAX_DATA_SIZE, "Msg #%d", counter);
        // msg->id = counter;
        // msg->len = strlen(msg->data) + 1;

        // msg->len = accelerometer_data2json(msg->data, a);
        msg->len = gyroscope_data2json(msg->data, a);

        // IMU_data_t imu;
        // imu.g = g;
        // imu.a = a;

        // msg->len = IMU_data2json(msg->data, a);
    }

    if (xRingbufferSend(ws_message_ringbuf, &msg, sizeof(msg), pdMS_TO_TICKS(100)) != pdTRUE)
    {
        ESP_LOGI(TAG, "Send failed");
        free_ws_msg(msg);
        return ESP_ERR
    }
    //--------------------

    return ESP_OK;
}
//----------------------------------------------------------------------
// Создание backend - структуры с персонализированными функциями драйвера
const i2c_device_backend_t _MPU6050_backend = {
    MPU6050_init,
    MPU6050_read};
//----------------------------------------------------------------------
esp_err_t MPU6050_setup(i2c_device_t *device)
{
    ESP_LOGI(TAG, "start setup");

    device->type_name = "MPU6050";
    device->backend = &_MPU6050_backend;

    return ESP_OK;
}