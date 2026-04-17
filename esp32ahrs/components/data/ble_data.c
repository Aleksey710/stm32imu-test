#include "ble_data.h"

#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

#include "config_bluetooth.h"
//----------------------------------------------------------------------
QueueHandle_t ble_queue = NULL;
//----------------------------------------------------------------------
/*
char json[128];

snprintf(json, sizeof(json),
    "{\"x\":%.2f,\"y\":%.2f,\"z\":%.2f}",
    roll, pitch, yaw);

xQueueSend(ble_queue, &json, 0);
*/
//----------------------------------------------------------------------