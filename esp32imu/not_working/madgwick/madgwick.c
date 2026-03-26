#include "madgwick.h"

void madgwick_update(imu_data_t *d)
{
    // упрощённый mock (замени на реальный алгоритм)
    d->qw = 1.0f;
    d->qx = d->ax * 0.01f;
    d->qy = d->ay * 0.01f;
    d->qz = 0;
}
