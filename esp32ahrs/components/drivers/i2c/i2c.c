#include "i2c.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"

#include "driver/i2c_master.h"

#include "config.h"
#include "data.h"

//-----------------------------------
// не менять последовательность
#include "i2c_device_private.h"
#include "i2c_device.h"
//-----------------------------------

#include "i2c_devices.h"
#include "MPU6050.h"
//----------------------------------------------------------------------
#define TAG "I2C"

static i2c_master_bus_handle_t bus;
static i2c_device_t *devices[MAX_DEVICES];
static int device_count = 0;

//----------------------------------------------------------------------
static bool i2c_probe(uint8_t addr)
{
    i2c_device_config_t dev_cfg = {
        .device_address = addr,
        .scl_speed_hz = I2C_FREQ,
    };

    i2c_master_dev_handle_t dev;
    if (i2c_master_bus_add_device(bus, &dev_cfg, &dev) != ESP_OK)
        return false;

    uint8_t data;
    esp_err_t ret = i2c_master_receive(dev, &data, 1, 50);

    i2c_master_bus_rm_device(dev);

    return ret == ESP_OK;
}
//----------------------------------------------------------------------
static void i2c_register_device(uint8_t addr)
{
    if (device_count >= MAX_DEVICES)
        return;

    // Создать абстрактное i2c устройство
    i2c_device_t *i2c_device = calloc(1, sizeof(i2c_device_t));
    if (!i2c_device)
        return;

    // Создать i2c конфигурацию устройства
    i2c_device_config_t dev_cfg = {
        .device_address = addr,
        .scl_speed_hz = I2C_FREQ,
    };

    if (i2c_master_bus_add_device(bus, &dev_cfg, &i2c_device->dev) != ESP_OK)
    {
        free(i2c_device);
        return;
    }

    i2c_device->dev_cfg = dev_cfg;

    i2c_device_init(i2c_device);

    devices[device_count++] = i2c_device;

    ESP_LOGI(TAG, "Registered device %s at 0x%02X", i2c_device->type_name, addr);
}
//----------------------------------------------------------------------
void i2c_scan_and_register(void)
{
    ESP_LOGI(TAG, "Scanning I2C bus...");

    for (uint8_t addr = 1; addr < 127; addr++)
    {
        if (i2c_probe(addr))
        {
            ESP_LOGI(TAG, "Found device at 0x%02X", addr);
            i2c_register_device(addr);
        }
    }
}
//----------------------------------------------------------------------
static void i2c_device_task(void *arg)
{
    device_data_t data;

    while (1)
    {
        for (int i = 0; i < device_count; i++)
        {
            i2c_device_t *device = devices[i];

            if (device->backend->read(device, &data) == ESP_OK)
            {
                // ESP_LOGI(TAG,
                //          "[%s] x=%.2f y=%.2f z=%.2f",
                //          device->backend->name,
                //          data.x, data.y, data.z);

                // 👉 сюда можно отправку в очередь добавить
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
//----------------------------------------------------------------------
void i2c_init(const int cpuid)
{
    ESP_LOGI(TAG, "Init I2C");

    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = I2C_PORT,
        .sda_io_num = I2C_SDA,
        .scl_io_num = I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .flags.enable_internal_pullup = true,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus));

    i2c_scan_and_register();

    xTaskCreatePinnedToCore(i2c_device_task,
                            "i2c_device_task",
                            4096,
                            NULL,
                            5,
                            NULL,
                            cpuid);
}