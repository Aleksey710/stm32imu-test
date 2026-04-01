#ifndef WEBSERVER_H
#define WEBSERVER_H
//----------------------------------------------------------------------
#include <inttypes.h>

#include "freertos/FreeRTOS.h" // ВСЕГДА ПЕРВЫЙ
#include "freertos/queue.h"
#include "freertos/task.h"

//----------------------------------------------------------------------
static QueueHandle_t ws_queue;

// ESP32 имеет аппаратный FPU только для float(32 - bit)
// float → работает на аппаратуре(быстро)
// double → считается программно(медленно)
typedef struct
{
    // int16_t ax, ay, az;
    float ax, ay, az;

    // int16_t gx, gy, gz;
    float gx, gy, gz;
    //
    // int16_t x, y, z;
    float x, y, z;

    // int16_t roll, pitch, yaw;
    float roll, pitch, yaw;

    // при форматировании не более 12 аргументов. Этот 13й
    //  uint64_t timestamp;

    // Use the correct format specifier macro :
    // For int64_t(signed), use % " PRId64 ".
    // For uint64_t(unsigned), use % " PRIu64 ".

    // % f
    // % lf // (в printf НЕ нужен, это для scanf)
    // % Lf // long double

} mpu_data_t;

//----------------------------------------------------------------------
void start_webserver(QueueHandle_t queue, const int cpuid);
//----------------------------------------------------------------------
#endif // WEBSERVER_H