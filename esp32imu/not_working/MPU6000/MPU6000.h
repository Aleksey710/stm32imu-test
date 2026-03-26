#pragma once
#include <stdint.h>
#include "esp_log.h"

static const char *TAG = "MODULE_NAME";

typedef struct {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
} mpu_data_t;

void i2c_task(void *pv);
