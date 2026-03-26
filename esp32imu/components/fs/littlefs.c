#include "littlefs.h"

#include "esp_littlefs.h"

//----------------------------------------------------------------------
void littlefs_init(void)
{
    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/fs",
        .partition_label = "storage",
        .format_if_mount_failed = true,
        .dont_mount = false};

    esp_err_t ret = esp_vfs_littlefs_register(&conf);

    if (ret != ESP_OK)
    {
        printf("LittleFS mount failed\n");
        return;
    }

    size_t total = 0, used = 0;
    esp_littlefs_info("storage", &total, &used);

    printf("LittleFS: total=%d, used=%d\n", total, used);
}
//----------------------------------------------------------------------