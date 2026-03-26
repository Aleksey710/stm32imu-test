#pragma once

typedef struct {
    float ax, ay, az;
    float qx, qy, qz, qw;
} imu_data_t;

void imu_init();
void imu_update();
imu_data_t imu_get();
