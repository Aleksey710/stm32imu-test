#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include <string.h>
#include <stdio.h>
//----------------------------------------------------------------------
#define POOL_SIZE 10

static Message_t pool[POOL_SIZE];
static bool used[POOL_SIZE];
//----------------------------------------------------------------------
Message_t* alloc_msg(void)
{
    for (int i = 0; i < POOL_SIZE; i++) {
        if (!used[i]) {
            used[i] = true;
            return &pool[i];
        }
    }
    return NULL;
}
//----------------------------------------------------------------------
void free_msg(Message_t *msg)
{
    int index = msg - pool;
    used[index] = false;
}
//----------------------------------------------------------------------
void producer_task(void *arg)
{
    int counter = 0;

    while (1) {
        Message_t *msg = alloc_msg();

        if (!msg) {
            printf("Pool empty\n");
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        snprintf(msg->data, MAX_DATA_SIZE, "Msg #%d", counter);
        msg->id = counter;
        msg->len = strlen(msg->data) + 1;

        if (xRingbufferSend(ringbuf, &msg, sizeof(msg), pdMS_TO_TICKS(100)) != pdTRUE) {
            printf("Send failed\n");
            free_msg(msg);
        } else {
            printf("Sent: %s\n", msg->data);
        }

        counter++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
//----------------------------------------------------------------------
void consumer_task(void *arg)
{
    while (1) {
        size_t size;
        Message_t **msg_ptr = (Message_t **)xRingbufferReceive(
            ringbuf, &size, portMAX_DELAY
        );

        if (msg_ptr) {
            Message_t *msg = *msg_ptr;

            printf("Received: %s\n", msg->data);

            free_msg(msg); // возвращаем в пул
            vRingbufferReturnItem(ringbuf, msg_ptr);
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
//----------------------------------------------------------------------
