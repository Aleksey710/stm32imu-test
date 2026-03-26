#include "fs.h"

#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_littlefs.h"

#include "config_fs.h"
//----------------------------------------------------------------------
static const char *TAG = "FS";
//----------------------------------------------------------------------
void spiffs_init(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = FS_BASE_PATH,
        .partition_label = FS_PARTITION_LABEL,
        .max_files = 5,
        .format_if_mount_failed = true};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "SPIFFS mount failed");
        return;
    }

    ESP_LOGE(TAG, "SPIFFS mounted");
}
//----------------------------------------------------------------------
void littlefs_init(void)
{
    esp_vfs_littlefs_conf_t conf = {
        .base_path = FS_BASE_PATH,
        .partition_label = FS_PARTITION_LABEL,
        .format_if_mount_failed = true,
        .dont_mount = false};

    esp_err_t ret = esp_vfs_littlefs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find LittleFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    esp_littlefs_info("storage", &total, &used);

    printf("LittleFS: total=%d, used=%d\n", total, used);
}
//----------------------------------------------------------------------
