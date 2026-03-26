#include "ws_server.h"
#include "esp_http_server.h"

static esp_err_t ws_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        return ESP_OK;
    }

    httpd_ws_frame_t frame = {0};
    frame.type = HTTPD_WS_TYPE_TEXT;

    httpd_ws_recv_frame(req, &frame, 0);

    frame.payload = malloc(frame.len);
    httpd_ws_recv_frame(req, &frame, frame.len);

    // echo
    httpd_ws_send_frame(req, &frame);

    free(frame.payload);
    return ESP_OK;
}

void ws_server_task(void *pv)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    httpd_start(&server, &config);

    httpd_uri_t ws = {
        .uri = "/ws",
        .method = HTTP_GET,
        .handler = ws_handler,
        .is_websocket = true
    };

    httpd_register_uri_handler(server, &ws);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
