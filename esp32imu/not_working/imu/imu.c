#include "imu.h"
#include "madgwick.h"

static imu_data_t data;

void imu_init()
{
    data.qw = 1.0f;
}

void imu_update()
{
    // тестовые данные
    data.ax += 0.1f;
    data.ay += 0.05f;
    data.az = 9.8f;

    madgwick_update(&data);
}

imu_data_t imu_get()
{
    return data;
}
