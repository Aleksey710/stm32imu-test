#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

#include "fs.h"

#include "wifi.h"
#include "webserver.h"
#include "i2c.h"

// #include "ws_server.h"

// #include "processing_task.h"
//----------------------------------------------------------------------
#include <mpu6050.h>
#include "mpu6050_test.h"
//----------------------------------------------------------------------
QueueHandle_t i2c_queue;

static const char *TAG = "MAIN ";
//----------------------------------------------------------------------
void app_main(void)
{
    ESP_LOGI(TAG, "************************************************************************");
    ESP_LOGI(TAG, "***                    START                                         ***");
    ESP_LOGI(TAG, "************************************************************************");
    //------------------------------------------------------------------
    // Очередь для передачи данных между задачами
    // перейти на ring buffer / stream buffer(лучше для телеметрии)
    i2c_queue = xQueueCreate(10, sizeof(int));

    ws_queue = xQueueCreate(10, sizeof(mpu_data_t));

    int cpuid;
    //------------------------------------------------------------------
    // --- CORE 0 ---
    cpuid = 0;

    ESP_LOGI(TAG, "CORE 0 initing");

    littlefs_init();
    // spiffs_init();

    wifi_init();

    start_webserver(ws_queue, cpuid);

    //     xTaskCreatePinnedToCore(webserver_task, "webserver_task", 8192, NULL, 5, NULL, 0);
    // xTaskCreatePinnedToCore(ws_server_task, "ws_task", 8192, NULL, 5, NULL, 0);

    //------------------------------------------------------------------
    // --- CORE 1 ---
    cpuid = 1;
    ESP_LOGI(TAG, "CORE 1 initing");

    // i2c_sensor_manager_start(cpuid);
    // i2c_init(cpuid);
    /*
    xTaskCreatePinnedToCore(i2c_task, "i2c_task", 4096, (void*)i2c_queue, 6, NULL, 1);
    xTaskCreatePinnedToCore(processing_task, "processing_task", 4096, (void*)i2c_queue, 5, NULL, 1);
    */
    //------------------------------------------------------------------
    // mpu6050_test
    mpu6050_test_start(ws_queue, cpuid);
    //------------------------------------------------------------------
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