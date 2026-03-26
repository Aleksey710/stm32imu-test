#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// #include "spiffs.h"
#include "littlefs.h"

#include "wifi.h"
#include "webserver.h"
// #include "ws_server.h"
// #include "i2c_task.h"
// #include "processing_task.h"

QueueHandle_t i2c_queue;
QueueHandle_t queue;
//----------------------------------------------------------------------
void app_main(void)
{
    // Очередь для передачи данных между задачами
    i2c_queue = xQueueCreate(10, sizeof(int));

    // --- CORE 0 ---

    littlefs_init();
    // spiffs_init();

    wifi_init();
    start_webserver(queue);

    //     xTaskCreatePinnedToCore(webserver_task, "webserver_task", 8192, NULL, 5, NULL, 0);
    // xTaskCreatePinnedToCore(ws_server_task, "ws_task", 8192, NULL, 5, NULL, 0);

    /*
        // --- CORE 1 ---
        xTaskCreatePinnedToCore(i2c_task, "i2c_task", 4096, (void*)i2c_queue, 6, NULL, 1);
        xTaskCreatePinnedToCore(processing_task, "processing_task", 4096, (void*)i2c_queue, 5, NULL, 1);
        */
}
//----------------------------------------------------------------------
/*
#include "freertos/ringbuf.h"

RingbufHandle_t raw_rb;
RingbufHandle_t processed_rb;

void app_main(void)
{
    raw_rb = xRingbufferCreate(4096, RINGBUF_TYPE_BYTEBUF);
    processed_rb = xRingbufferCreate(4096, RINGBUF_TYPE_BYTEBUF);

    wifi_init();
    start_webserver(processed_rb);

    xTaskCreatePinnedToCore(i2c_task, "i2c", 4096, raw_rb, 6, NULL, 1);
    xTaskCreatePinnedToCore(processing_task, "proc", 4096,
                            (void*) (RingbufHandle_t[]){raw_rb, processed_rb},
                            5, NULL, 1);
}
*/