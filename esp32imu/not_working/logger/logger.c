/*
#include "esp_log.h"

static const char *TAG = "MODULE_NAME";

Пример :

    ESP_LOGI(TAG, "Init done");
ESP_LOGW(TAG, "Something suspicious");
ESP_LOGE(TAG, "Error: %d", err);

👉 Рекомендация :

I — нормальные события
W — нестабильные состояния
E — ошибки
D — только при отладке
*/