#include "webserver.h"

#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "esp_http_server.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"

#include "cJSON.h"

#include "config.h"
#include "fs.h"
#include "IMU_data.h"
#include "ws_msg_ringbuf.h"

//----------------------------------------------------------------------
#define MAX_WS_CLIENTS 8

static const char *TAG = "WEBSERVER";

static httpd_handle_t server = NULL;

// Массив для хранения fd WebSocket клиентов
static int ws_clients[MAX_WS_CLIENTS];
static SemaphoreHandle_t ws_clients_mutex = NULL;

//----------------------------------------------------------------------
static void ws_add_client(const int fd)
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
static void ws_remove_client(const int fd)
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
/*
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
*/
//----------------------------------------------------------------------
static void set_content_type(httpd_req_t *req, const char *filepath)
{
    if (strstr(filepath, ".html"))
        httpd_resp_set_type(req, "text/html");
    else if (strstr(filepath, ".js"))
        httpd_resp_set_type(req, "application/javascript");
    else if (strstr(filepath, ".css"))
        httpd_resp_set_type(req, "text/css");
    else if (strstr(filepath, ".json"))
        httpd_resp_set_type(req, "application/json");
}
//----------------------------------------------------------------------
static esp_err_t file_get_handler(httpd_req_t *req)
{
    // ESP_LOGI(TAG, "file_get_handler_1 req[%s]", req->uri);

    //------------------------------------------------------------------
    // Проверка длинны пути файла
    if (strlen(req->uri) > (FILEPATH_MAX - 3))
    {
        httpd_resp_send_err(req, HTTPD_414_URI_TOO_LONG, "URI too long");
        return ESP_FAIL;
    }

    //------------------------------------------------------------------
    // защита от ../ (path traversal):
    if (strstr(req->uri, ".."))
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid path");
        return ESP_FAIL;
    }

    //------------------------------------------------------------------
    char filepath[FILEPATH_MAX];
    int filepath_len = 0;

    // если корень — отдаём index.html
    if (strcmp(req->uri, "/") == 0 ||
        strcmp(req->uri, "/index.htm") == 0 ||
        strcmp(req->uri, "/index.html") == 0)
    {
        filepath_len = snprintf(filepath, sizeof(filepath), "%s/index.html", FS_WEBSERVER_PATH);
    }
    else // иначе ищем в файловой системе
    {
        filepath_len = snprintf(filepath, sizeof(filepath), "%s%s", FS_WEBSERVER_PATH, req->uri);
    }

    if (strcmp(req->uri, "/favicon.ico") == 0)
    {
        httpd_resp_set_status(req, "204 No Content");
        httpd_resp_send(req, NULL, 0);
        return ESP_OK;
    }

    if (filepath_len < 0 || filepath_len >= sizeof(filepath))
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Path too long");
        return ESP_FAIL;
    }

    // ESP_LOGI(TAG, "file_get_handler_2 filepath[%s]", filepath);

    //------------------------------------------------------------------
    char gz_path[FILEPATH_MAX];
    int gz_len = snprintf(gz_path, sizeof(gz_path), "%s.gz", filepath);

    if (gz_len < 0 || gz_len >= sizeof(gz_path))
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Path gz too long");
        return ESP_FAIL;
    }

    // ESP_LOGI(TAG, "file_get_handler_3 gz_path[%s]", gz_path);

    //------------------------------------------------------------------
    FILE *f = nullptr;
    if (f)
    {
        f = fopen(gz_path, "r");
        httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    }
    else
    {
        f = fopen(filepath, "r");
    }

    if (!f)
    {

        // {
        //     // SPA fallback → всегда index.html
        //     snprintf(filepath, sizeof(filepath),
        //              "%s/index.html", FS_WEBSERVER_PATH);

        //     f = fopen(filepath, "r");
        // }

        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    //------------------------------------------------------------------
    // вставляем MIME-типы
    set_content_type(req, filepath);

    //------------------------------------------------------------------
    // Добавление кэширования файлов
    if (strstr(filepath, ".html"))
    {
        httpd_resp_set_hdr(req, "Cache-Control", "no-cache, no-store, must-revalidate");
    }
    else if (
        strstr(filepath, ".js") ||
        strstr(filepath, ".css"))
    {
        httpd_resp_set_hdr(req, "Cache-Control", "public, max-age=31536000, immutable");
    }

    //------------------------------------------------------------------
    char chunk[1024];
    size_t read_bytes;

    while ((read_bytes = fread(chunk, 1, sizeof(chunk), f)) > 0)
    {
        httpd_resp_send_chunk(req, chunk, read_bytes);
    }

    fclose(f);

    ESP_LOGI(TAG, "file_get_handler_4 req[%s] resp[%s]", req->uri, filepath);

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

    // while (1)
    // {
    //     esp_err_t ret = httpd_ws_recv_frame(req, &frame, 0);
    //     if (ret != ESP_OK)
    //     {
    //         ws_remove_client(fd);
    //         break;
    //     }
    // // yeld - освобождение потока
    // vTaskDelay(pdMS_TO_TICKS(1));
    // }
    return ESP_OK;
}
//----------------------------------------------------------------------
void send_ws_msg(char *json_str, int json_str_len)
{
    httpd_ws_frame_t frame = {
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = (uint8_t *)json_str,
        .len = json_str_len};

    xSemaphoreTake(ws_clients_mutex, portMAX_DELAY);
    for (int i = 0; i < MAX_WS_CLIENTS; i++)
    {
        if (ws_clients[i] != -1)
        {
            esp_err_t ret = httpd_ws_send_frame_async(server, ws_clients[i], &frame);

            ESP_LOGI(TAG, "ws_sender_task sended[%s]", json_str);
            if (ret != ESP_OK)
            {
                ws_clients[i] = -1; // клиент отключился
                ESP_LOGI(TAG, "Removed dead WS client fd=%d", ws_clients[i]);
            }
        }
    }
    xSemaphoreGive(ws_clients_mutex);
}
//----------------------------------------------------------------------
static void ws_sender_task(void *arg)
{
    while (1)
    {
        size_t size;
        Ws_msg_t **msg_ptr = (Ws_msg_t **)xRingbufferReceive(
            ws_msg_ringbuf, &size, portMAX_DELAY);

        if (msg_ptr)
        {
            Ws_msg_t *msg = *msg_ptr;
            //--------------------
            // ESP_LOGI(TAG, "Received: %s", msg->data);

            send_ws_msg(msg->data->data, int msg->data->len);

            //--------------------
            free_ws_msg(msg); // возвращаем в пул
            vRingbufferReturnItem(ws_msg_ringbuf, msg_ptr);
        }

        // ограничение частоты посылки
        // vTaskDelay(pdMS_TO_TICKS(20)); // 20 - 50 Hz

        // yeld - освобождение потока
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    /*

        // mpu_data_t test_data;

    // test_data.ax = 1.0f;
    // test_data.ay = 2.0f;
    // test_data.az = 3.0f;

    // test_data.gx = 0.1f;
    // test_data.gy = 0.2f;
    // test_data.gz = 0.3f;

    // test_data.x = 10.1f;
    // test_data.y = 20.2f;
    // test_data.z = 30.3f;

    // test_data.roll = 10.1f;
    // test_data.pitch = 20.2f;
    // test_data.yaw = 30.3f;

    // // test_data.timestamp = 0;

    // xQueueSend(ws_queue, &test_data, portMAX_DELAY);
    // //---------
    // mpu_data_t data;

    while (1)
    {
        //--------------------
        if (xQueueReceive(ws_queue, &data, portMAX_DELAY))
        {

            char json_str[256];

            int json_str_len = snprintf(json_str, sizeof(json_str),
                                        "{"
                                        "\"ax\":" FMT_F ",\"ay\":" FMT_F ",\"az\":" FMT_F ","
                                        "\"gx\":" FMT_F ",\"gy\":" FMT_F ",\"gz\":" FMT_F ","
                                        "\"x\":" FMT_F ",\"y\":" FMT_F ",\"z\":" FMT_F ","
                                        "\"roll\":" FMT_F ",\"pitch\":" FMT_F ",\"yaw\":" FMT_F
                                        "}",
                                        (double)data.ax, (double)data.ay, (double)data.az,
                                        (double)data.gx, (double)data.gy, (double)data.gz,
                                        (double)data.x, (double)data.y, (double)data.z,
                                        (double)data.roll, (double)data.pitch, (double)data.yaw);

            send_ws_msg(json_str, json_str_len);

            vTaskDelay(pdMS_TO_TICKS(1));
        }
        // ограничение частоты посылки
        vTaskDelay(pdMS_TO_TICKS(20)); // 20 - 50 Hz

        // yeld - освобождение потока
        vTaskDelay(pdMS_TO_TICKS(1));
    }
        */
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
void start_webserver(QueueHandle_t queue, const int cpuid)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // разрешает wildcard
    config.uri_match_fn = httpd_uri_match_wildcard;

    if (httpd_start(&server, &config) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return;
    }

    //------------------------------------------------------------------
    ws_queue = queue;

    ws_clients_mutex = xSemaphoreCreateMutex();
    if (!ws_clients_mutex)
    {
        ESP_LOGE(TAG, "Failed to create WS clients mutex");
        return;
    }

    for (int i = 0; i < MAX_WS_CLIENTS; i++)
    {
        ws_clients[i] = -1;
    }

    //------------------------------------------------------------------
    // Обработчики регистрируются ниже и обрабатываются
    // в порядке очередности регистрации.
    // Обработчик .uri = "/*" вначале перекрыл бы все, по этому обрабатывается в конце
    //------------------------------------------------------------------

    // WebSocket endpoint
    httpd_uri_t ws = {
        .uri = "/ws",
        .method = HTTP_GET,
        .handler = ws_handler,
        .is_websocket = true};

    httpd_register_uri_handler(server, &ws);

    //------------------------------------------------------------------
    // root endpoint
    httpd_uri_t root = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = file_get_handler};

    httpd_register_uri_handler(server, &root);

    httpd_uri_t index = {
        .uri = "/index.html",
        .method = HTTP_GET,
        .handler = file_get_handler};

    httpd_register_uri_handler(server, &index);

    //------------------------------------------------------------------
    // ***
    //------------------------------------------------------------------
    // Все остальное, что не было зарегестрировано ранее
    httpd_uri_t file_uri = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = file_get_handler,
    };

    httpd_register_uri_handler(server, &file_uri);

    //------------------------------------------------------------------
    // xTaskCreate(ws_sender_task, "ws_sender", 4096, NULL, 5, NULL);
    // xTaskCreatePinnedToCore(ws_sender_task, "ws_sender", 4096, NULL, 5, NULL, cpuid);
    xTaskCreatePinnedToCore(ws_sender_task, "ws_sender", 8192, NULL, 5, NULL, cpuid);

    // xTaskCreate(ws_ping_task, "ws_ping", 2048, NULL, 5, NULL);
    // xTaskCreatePinnedToCore(ws_ping_task, "ws_ping", 2048, NULL, 5, NULL, cpuid);
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