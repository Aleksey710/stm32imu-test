#include "spiffs.h"

#include "esp_spiffs.h"
//----------------------------------------------------------------------
void spiffs_init(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        printf("SPIFFS mount failed\n");
        return;
    }

    printf("SPIFFS mounted\n");
}
//----------------------------------------------------------------------
