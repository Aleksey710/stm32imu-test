/*
✅ Плюсы
🔥 нет копирования вообще
🔥 нет malloc
🔥 максимум производительности
идеально для:
UART DMA
I2S
high-throughput
❗ Минусы
сложнее API
нельзя “передумать” после Acquire
нужно строго вызывать SendComplete


👉 Zero-copy бери если:

важна скорость
большие данные
поток (аудио, UART, DMA)

👉 Object pool бери если:

сложные структуры
хочешь полный контроль
нужна переиспользуемая память
💡 Практический совет

В реальных проектах на ESP32 часто делают так:

данные → zero-copy ringbuffer
управление/события → queue
сложные объекты → pool
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include <string.h>
#include <stdio.h>

//----------------------------------------------------------------------
static RingbufHandle_t ringbuf;
//----------------------------------------------------------------------
#define MAX_DATA_SIZE 32

typedef struct {
    int id;
    size_t len;
    char data[MAX_DATA_SIZE];
} Message_t;
//----------------------------------------------------------------------
//----------------------------------------------------------------------
void producer_task(void *arg)
{
    int counter = 0;

    while (1) {
        char text[32];
        snprintf(text, sizeof(text), "Msg #%d", counter);

        size_t len = strlen(text) + 1;
        size_t total_size = sizeof(Message_t) - MAX_DATA_SIZE + len;

        // 1. Получаем память внутри ringbuffer
        Message_t *msg = (Message_t *)xRingbufferSendAcquire(
            ringbuf,
            total_size,
            pdMS_TO_TICKS(100)
        );

        if (msg == NULL) {
            printf("Acquire failed\n");
            continue;
        }

        // 2. Заполняем прямо в буфере
        msg->id = counter;
        msg->len = len;
        memcpy(msg->data, text, len);

        // 3. Завершаем отправку
        xRingbufferSendComplete(ringbuf, msg);

        printf("Sent: %s\n", text);

        counter++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
//----------------------------------------------------------------------
void consumer_task(void *arg)
{
    while (1) {
        size_t item_size;

        Message_t *msg = (Message_t *)xRingbufferReceive(
            ringbuf,
            &item_size,
            pdMS_TO_TICKS(5000)
        );

        if (msg != NULL) {
            printf("Received ID=%d, Data=%s\n", msg->id, msg->data);

            vRingbufferReturnItem(ringbuf, msg);
        } else {
            printf("Timeout waiting for data\n");
        }
    }
}

//----------------------------------------------------------------------

void app_main(void)
{
    ringbuf = xRingbufferCreate(2048, RINGBUF_TYPE_NOSPLIT);

    if (ringbuf == NULL) {
        printf("Failed to create ring buffer\n");
        return;
    }

    xTaskCreate(producer_task, "producer", 4096, NULL, 5, NULL);
    xTaskCreate(consumer_task, "consumer", 4096, NULL, 5, NULL);
}

















