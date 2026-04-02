#include "mpu6050_test.h"

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/ringbuf.h"
#include <esp_err.h>
#include <esp_log.h>
#include <mpu6050.h>

#include "config.h"
#include "i2c_devices.h"

#include "accelerometer_data.h"
#include "gyroscope_data.h"

#include "ws_msg_ringbuf.h"
// #include "webserver.h"
//----------------------------------------------------------------------

static const char *TAG = "mpu6050_test";

//----------------------------------------------------------------------
static QueueHandle_t ws_queue;
//----------------------------------------------------------------------
void mpu6050_test(void *pvParameters)
{
    mpu6050_dev_t dev = {0};

    ESP_ERROR_CHECK(mpu6050_init_desc(&dev,
                                      DEVICE_ADDRESS::MPU6050_LOW,
                                      0,
                                      I2C_SDA_GPIO,
                                      I2C_SCL_GPIO));

    while (1)
    {
        esp_err_t res = i2c_dev_probe(&dev.i2c_dev, I2C_DEV_WRITE);
        if (res == ESP_OK)
        {
            ESP_LOGI(TAG, "Found MPU60x0 device");
            break;
        }
        ESP_LOGE(TAG, "MPU60x0 not found");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_ERROR_CHECK(mpu6050_init(&dev));

    ESP_LOGI(TAG, "Accel range: %d", dev.ranges.accel);
    ESP_LOGI(TAG, "Gyro range:  %d", dev.ranges.gyro);

    //-------------
    // mpu_data_t test_data;
    //-------------
    while (1)
    {
        float temp;
        mpu6050_acceleration_t accel = {0};
        mpu6050_rotation_t rotation = {0};

        ESP_ERROR_CHECK(mpu6050_get_temperature(&dev, &temp));
        ESP_ERROR_CHECK(mpu6050_get_motion(&dev, &accel, &rotation));

        Gyroscope_unit_data_t g;
        g.x = rotation.x;
        g.y = rotation.y;
        g.z = rotation.z;

        Accelerometer_unit_data_t a;
        a.x = accel.x;
        a.y = accel.y;
        a.z = accel.z;

        // ESP_LOGI(TAG, "**********************************************************************");
        // ESP_LOGI(TAG, "Acceleration: x=%.4f   y=%.4f   z=%.4f", accel.x, accel.y, accel.z);
        // ESP_LOGI(TAG, "Rotation:     x=%.4f   y=%.4f   z=%.4f", rotation.x, rotation.y, rotation.z);
        // ESP_LOGI(TAG, "Temperature:  %.1f", temp);
        /*
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
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
//----------------------------------------------------------------------
void mpu6050_test_start(QueueHandle_t queue, const int cpuid)
{
    ESP_ERROR_CHECK(i2cdev_init());

    ws_queue = queue;

    // xTaskCreate(mpu6050_test, "mpu6050_test", configMINIMAL_STACK_SIZE * 6, NULL, 5, NULL);
    // xTaskCreatePinnedToCore(mpu6050_test, "mpu6050_test", configMINIMAL_STACK_SIZE * 6, NULL, 5, NULL, cpuid);
    xTaskCreatePinnedToCore(mpu6050_test, "mpu6050_test", 8192, NULL, 5, NULL, cpuid);
}
