#include "i2c_task.h"
#include "driver/i2c.h"

void i2c_task(void *pv)
{
    QueueHandle_t queue = (QueueHandle_t)pv;
    int data = 0;

    while (1) {
        // имитация чтения
        data++;

        xQueueSend(queue, &data, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
