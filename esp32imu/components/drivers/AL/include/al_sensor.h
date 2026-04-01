#ifndef AL_SENSOR_H
#define AL_SENSOR_H
//----------------------------------------------------------------------
#include <inttypes.h>

// #include "freertos/FreeRTOS.h" // ВСЕГДА ПЕРВЫЙ
// #include "freertos/queue.h"
// #include "freertos/task.h"

//----------------------------------------------------------------------
// SENSOR INTERFACE
//----------------------------------------------------------------------
// Объявление нового типа данных [al_sensor_t] - [al_struct sensor_t]
typedef struct al_sensor_t al_sensor_t;

// Объявление и определение нового типа данных sensor_ops_t
// с абстрактными функциями для инициализации и чтения данных
typedef struct
{
    esp_err_t (*init)(al_sensor_t *sensor);
    esp_err_t (*read)(al_sensor_t *sensor, void *data);
    const char *name;
} al_sensor_ops_t;

// Объявление и определение структуры с данными датчика
struct al_sensor_t
{
    uint8_t address;
    al_sensor_ops_t *ops;
    void *ctx; // внутренние данные драйвера
};

/*
//--------------------------------------------------
// SENSOR STORAGE
//--------------------------------------------------
#define MAX_SENSORS 10

static sensor_t *sensors[MAX_SENSORS];
static int sensor_count = 0;

//--------------------------------------------------
// ADD SENSOR
//--------------------------------------------------
void sensor_add(sensor_t *sensor)
{
    if (sensor_count >= MAX_SENSORS)
    {
        ESP_LOGW(TAG, "Sensor list full");
        return;
    }

    sensors[sensor_count++] = sensor;

    ESP_LOGI(TAG, "Adding sensor: %s (0x%02X)",
             sensor->ops->name, sensor->address);

    sensor->ops->init(sensor);
}

//--------------------------------------------------
// EXAMPLE SENSOR (DUMMY / MPU MOCK)
//--------------------------------------------------
typedef struct
{
    int dummy;
} mpu_ctx_t;

esp_err_t mpu_init(sensor_t *s)
{
    ESP_LOGI(TAG, "MPU init at 0x%02X", s->address);
    return ESP_OK;
}

esp_err_t mpu_read(sensor_t *s, void *data)
{
    float *f = (float *)data;

    // fake data
    f[0] = 1.0f;
    f[1] = 2.0f;
    f[2] = 3.0f;

    return ESP_OK;
}

sensor_ops_t mpu_ops = {
    .init = mpu_init,
    .read = mpu_read,
    .name = "MPU6050"};
*/
//----------------------------------------------------------------------
#endif // AL_SENSOR_H
