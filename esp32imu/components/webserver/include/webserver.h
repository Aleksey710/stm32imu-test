
#ifndef WEBSERVER_H
#define WEBSERVER_H
//----------------------------------------------------------------------
#include "freertos/FreeRTOS.h" // ВСЕГДА ПЕРВЫЙ
#include "freertos/queue.h"
#include "freertos/task.h"

//----------------------------------------------------------------------
void start_webserver(QueueHandle_t queue);
//----------------------------------------------------------------------
#endif // WEBSERVER_H