#include "processing_task.h"
#include "esp_log.h"

static const char *TAG = "MODULE_NAME";

void processing_task(void *pv)
{
    QueueHandle_t *queues = (QueueHandle_t*)pv;
    QueueHandle_t in = queues[0];
    QueueHandle_t out = queues[1];

    mpu_data_t data;

    while (1) {
        if (xQueueReceive(in, &data, portMAX_DELAY)) {

            // тут можно фильтровать (например low-pass)

            xQueueSend(out, &data, portMAX_DELAY);
        }
    }
}
//--------------------------------
size_t size;
mpu_data_t *data = (mpu_data_t*) xRingbufferReceive(rb_in, &size, portMAX_DELAY);

if (data) {
    // обработка
    xRingbufferSend(rb_out, data, size, portMAX_DELAY);
    vRingbufferReturnItem(rb_in, data);
}
