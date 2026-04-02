#include "ws_msg_ringbuf.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"

#include "esp_log.h"
#include "esp_err.h"

//----------------------------------------------------------------------
#define TAG "WS_RINGBUF"
//----------------------------------------------------------------------
static RingbufHandle_t ws_msg_ringbuf;
//----------------------------------------------------------------------

static Ws_msg_t ws_msg_pool[WS_MSG_POOL_SIZE];
static bool ws_msg_pool_used[WS_MSG_POOL_SIZE];
//----------------------------------------------------------------------
Ws_msg_t *alloc_ws_msg(void)
{
    Ws_msg_t *msg = NULL;

    for (int i = 0; i < WS_MSG_POOL_SIZE; i++)
    {
        if (!ws_msg_pool_used[i])
        {
            ws_msg_pool_used[i] = true;
            msg = &ws_msg_pool[i];
            break;
        }
    }

    if (!msg)
    {
        vTaskDelay(pdMS_TO_TICKS(1));
        ESP_LOGI(TAG, "Pool empty");
    }

    return msg;
}
//----------------------------------------------------------------------
void free_ws_msg(Ws_msg_t *msg)
{
    int index = msg - ws_msg_pool;
    ws_msg_pool_used[index] = false;
}
//----------------------------------------------------------------------
// положить сообщение в буффер

esp_err_t send_ws_msg_to_buf(const Ws_msg_data_t &data)
{
    Ws_msg_t *msg = alloc_ws_msg();

    // esp_err_t res;

    if (msg)
    {
        // snprintf(msg->data, WS_RINGBUFF_MAX_DATA_SIZE, "Msg #%d", counter);
        // msg->id = counter;
        // msg->len = strlen(msg->data) + 1;

        msg->data = data;
    }

    if (xRingbufferSend(ws_message_ringbuf, &msg, sizeof(msg), pdMS_TO_TICKS(100)) != pdTRUE)
    {
        ESP_LOGI(TAG, "Send failed");
        free_ws_msg(msg);
        return ESP_ERR
    }
    // else
    // {
    //     ESP_LOGI(TAG, "Sent: %s", msg->data);
    // }

    // counter++;
    // vTaskDelay(pdMS_TO_TICKS(1000));

    return ESP_OK;
}

void producer_task(void *arg)
{
    int counter = 0;

    while (1)
    {
        Ws_msg_data_t data;

        snprintf(data->data, WS_RINGBUFF_MAX_DATA_SIZE, "Msg #%d", counter);
        msg->id = counter;
        msg->len = strlen(msg->data) + 1;

        //-----------------------------
        send_ws_msg_to_buf();
        //---------------------------------

        counter++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
//----------------------------------------------------------------------
void consumer_task(void *arg)
{
    while (1)
    {
        size_t size;
        Ws_msg_t **msg_ptr = (Ws_msg_t **)xRingbufferReceive(
            ws_msg_ringbuf, &size, portMAX_DELAY);

        if (msg_ptr)
        {
            Ws_msg_t *msg = *msg_ptr;

            ESP_LOGI(TAG, "Received: %s", msg->data);

            free_ws_msg(msg); // возвращаем в пул
            vRingbufferReturnItem(ws_msg_ringbuf, msg_ptr);
        }
    }
}
//----------------------------------------------------------------------
void ws_msg_ringbuf_setup(void)
{
    ws_msg_ringbuf = xRingbufferCreate(2048, RINGBUF_TYPE_NOSPLIT);

    if (ws_msg_ringbuf == NULL)
    {
        ESP_LOGI(TAG, "Failed to create ring buffer: %s", msg->data);
        return;
    }

    // xTaskCreate(producer_task, "producer", 4096, NULL, 5, NULL);
    // xTaskCreate(consumer_task, "consumer", 4096, NULL, 5, NULL);
}
//----------------------------------------------------------------------
