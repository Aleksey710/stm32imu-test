#include "webserver.h"

#include "esp_http_server.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>
//----------------------------------------------------------------------
#define MAX_WS_CLIENTS 8

static const char *TAG = "WEBSERVER";

static httpd_handle_t server = NULL;
static QueueHandle_t ws_queue;

// Массив для хранения fd WebSocket клиентов
static int ws_clients[MAX_WS_CLIENTS];
static SemaphoreHandle_t ws_clients_mutex = NULL;

typedef struct
{
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
} mpu_data_t;

//----------------------------------------------------------------------
static void ws_add_client(int fd)
{
    xSemaphoreTake(ws_clients_mutex, portMAX_DELAY);
    for (int i = 0; i < MAX_WS_CLIENTS; i++)
    {
        if (ws_clients[i] == -1)
        {
            ws_clients[i] = fd;
            ESP_LOGI(TAG, "WS client added, fd=%d", fd);
            break;
        }
    }
    xSemaphoreGive(ws_clients_mutex);
}
//----------------------------------------------------------------------
static void ws_remove_client(int fd)
{
    xSemaphoreTake(ws_clients_mutex, portMAX_DELAY);
    for (int i = 0; i < MAX_WS_CLIENTS; i++)
    {
        if (ws_clients[i] == fd)
        {
            ws_clients[i] = -1;
            ESP_LOGI(TAG, "WS client removed, fd=%d", fd);
            break;
        }
    }
    xSemaphoreGive(ws_clients_mutex);
}
//----------------------------------------------------------------------
static esp_err_t root_get_handler(httpd_req_t *req)
{
    const char *html =
        "<!DOCTYPE html><html><body>"
        "<h2>MPU6000 Data</h2>"
        "<pre id='data'></pre>"
        "<script>"
        "let ws = new WebSocket('ws://' + location.host + '/ws');"
        "ws.onmessage = (e) => { document.getElementById('data').textContent = e.data; };"
        "</script></body></html>";

    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
//----------------------------------------------------------------------
static void set_content_type(httpd_req_t *req, const char *filepath)
{
    if (strstr(filepath, ".html"))
        httpd_resp_set_type(req, "text/html");
    else if (strstr(filepath, ".js"))
        httpd_resp_set_type(req, "application/javascript");
    else if (strstr(filepath, ".css"))
        httpd_resp_set_type(req, "text/css");
}
//----------------------------------------------------------------------
static esp_err_t file_get_handler(httpd_req_t *req)
{
    char filepath[128];
    snprintf(filepath, sizeof(filepath), "/spiffs%s", req->uri);

    char gz_path[128];
    snprintf(gz_path, sizeof(gz_path), "%s.gz", filepath);

    // // если корень — отдаём index.html
    // if (strcmp(req->uri, "/") == 0)
    // {
    //     strcpy(filepath, "/spiffs/index.html");
    // }

    FILE *f = fopen(gz_path, "r");
    if (f)
    {
        httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    }
    else
    {
        f = fopen(filepath, "r");
    }

    if (!f)
    {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    // Добавление кэширования для не ".html" (JS/CSS...) файлов
    if (strstr(filepath, ".html"))
    {
        httpd_resp_set_type(req, "text/html");
        httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
    }
    else
    {
        httpd_resp_set_hdr(req, "Cache-Control", "max-age=86400");
    }

    // вставляем MIME-типы
    set_content_type(req, filepath);

    // zip header
    if (is_gzip)
    {
        httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    }

    char chunk[1024];
    size_t read_bytes;

    while ((read_bytes = fread(chunk, 1, sizeof(chunk), f)) > 0)
    {
        httpd_resp_send_chunk(req, chunk, read_bytes);
    }

    fclose(f);

    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
//----------------------------------------------------------------------
static esp_err_t ws_handler(httpd_req_t *req)
{
    int fd = httpd_req_to_sockfd(req);
    ws_add_client(fd);

    // Ждём отключения клиента
    httpd_ws_frame_t frame = {0};
    frame.type = HTTPD_WS_TYPE_TEXT;

    while (1)
    {
        esp_err_t ret = httpd_ws_recv_frame(req, &frame, 0);
        if (ret != ESP_OK)
        {
            ws_remove_client(fd);
            break;
        }
    }
    return ESP_OK;
}
//----------------------------------------------------------------------
static void ws_sender_task(void *arg)
{
    mpu_data_t data;

    while (1)
    {
        if (xQueueReceive(ws_queue, &data, portMAX_DELAY))
        {

            cJSON *root = cJSON_CreateObject();
            cJSON_AddNumberToObject(root, "ax", data.ax);
            cJSON_AddNumberToObject(root, "ay", data.ay);
            cJSON_AddNumberToObject(root, "az", data.az);
            cJSON_AddNumberToObject(root, "gx", data.gx);
            cJSON_AddNumberToObject(root, "gy", data.gy);
            cJSON_AddNumberToObject(root, "gz", data.gz);

            char *json_str = cJSON_PrintUnformatted(root);
            cJSON_Delete(root);
            if (!json_str)
                continue;

            httpd_ws_frame_t frame = {
                .type = HTTPD_WS_TYPE_TEXT,
                .payload = (uint8_t *)json_str,
                .len = strlen(json_str)};

            xSemaphoreTake(ws_clients_mutex, portMAX_DELAY);
            for (int i = 0; i < MAX_WS_CLIENTS; i++)
            {
                if (ws_clients[i] != -1)
                {
                    esp_err_t ret = httpd_ws_send_frame_async(server, ws_clients[i], &frame);
                    if (ret != ESP_OK)
                    {
                        ws_clients[i] = -1; // клиент отключился
                        ESP_LOGI(TAG, "Removed dead WS client fd=%d", ws_clients[i]);
                    }
                }
            }
            xSemaphoreGive(ws_clients_mutex);

            free(json_str);
        }
    }
}
//----------------------------------------------------------------------
static void ws_ping_task(void *arg)
{
    const TickType_t delay = pdMS_TO_TICKS(10000); // каждые 10 секунд
    httpd_ws_frame_t ping_frame = {
        .type = HTTPD_WS_TYPE_PING,
        .payload = NULL,
        .len = 0};

    while (1)
    {
        vTaskDelay(delay);

        xSemaphoreTake(ws_clients_mutex, portMAX_DELAY);
        for (int i = 0; i < MAX_WS_CLIENTS; i++)
        {
            if (ws_clients[i] != -1)
            {
                esp_err_t ret = httpd_ws_send_frame_async(server, ws_clients[i], &ping_frame);
                if (ret != ESP_OK)
                {
                    ws_clients[i] = -1; // клиент отключился
                }
            }
        }
        xSemaphoreGive(ws_clients_mutex);
    }
}
//----------------------------------------------------------------------
void start_webserver(QueueHandle_t queue)
{
    ws_queue = queue;

    ws_clients_mutex = xSemaphoreCreateMutex();
    if (!ws_clients_mutex)
    {
        ESP_LOGE(TAG, "Failed to create WS clients mutex");
        return;
    }

    for (int i = 0; i < MAX_WS_CLIENTS; i++)
        ws_clients[i] = -1;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return;
    }

    //------------------------------------------------------------------
    // корневой URI
    // httpd_uri_t root = {
    //     .uri = "/",
    //     .method = HTTP_GET,
    //     .handler = root_get_handler};
    // httpd_register_uri_handler(server, &root);

    //------------------------------------------------------------------
    // файлы из SPIFFS
    httpd_uri_t file_uri = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = file_get_handler,
    };

    httpd_register_uri_handler(server, &file_uri);

    //------------------------------------------------------------------
    // WebSocket URI
    httpd_uri_t ws = {
        .uri = "/ws",
        .method = HTTP_GET,
        .handler = ws_handler,
        .is_websocket = true};
    httpd_register_uri_handler(server, &ws);

    //------------------------------------------------------------------
    // xTaskCreate(ws_sender_task, "ws_sender", 4096, NULL, 5, NULL);
    // xTaskCreate(ws_ping_task, "ws_ping", 2048, NULL, 5, NULL);
}
//----------------------------------------------------------------------
/*
static esp_err_t root_get_handler(httpd_req_t *req)
{
    const char *html =
        "<!DOCTYPE html><html><body>"
        "<h2>MPU6000 Data</h2>"
        "<pre id='data'></pre>"
        "<script>"
        "function startWS() {"
        "  let ws = new WebSocket('ws://' + location.host + '/ws');"
        "  ws.onmessage = (e) => { document.getElementById('data').textContent = e.data; };"
        "  ws.onclose = () => {"
        "    console.log('WS closed, reconnecting...');"
        "    setTimeout(startWS, 2000);"
        "  };"
        "  ws.onerror = (err) => { ws.close(); };"
        "}"
        "startWS();"
        "</script></body></html>";

    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
*/