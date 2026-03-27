#include "fs.h"

#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

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

    ESP_LOGI(TAG, "SPIFFS mounted");
}
//----------------------------------------------------------------------
void littlefs_init(void)
{
    esp_vfs_littlefs_conf_t conf = {
        .base_path = FS_BASE_PATH,
        .partition_label = FS_PARTITION_LABEL,
        .format_if_mount_failed = false,
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
    esp_littlefs_info(FS_PARTITION_LABEL, &total, &used);

    ESP_LOGI(TAG, "LittleFS: total=%d, used=%d", total, used);

    // Печать содержимого файловой системы
    list_dir(FS_BASE_PATH, 0);
}
//----------------------------------------------------------------------
void list_dir(const char *base_path, int level)
{
    DIR *dir = opendir(base_path);
    if (!dir)
    {
        ESP_LOGE(TAG, "Failed to open dir: %s", base_path);
        return;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {

        // пропускаем . и ..
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // создаём буфер для пути
        char path[256];

        // snprintf возвращает длину, которую он хотел записать
        int len = snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);

        // проверяем на переполнение
        if (len < 0 || len >= sizeof(path))
        {
            ESP_LOGW(TAG, "Path too long, skipping: %s/%s", base_path, entry->d_name);
            continue;
        }

        // делаем отступ
        char indent[64] = {0};
        for (int i = 0; i < level; i++)
        {
            strcat(indent, "  ");
        }

        struct stat st;
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
        {

            ESP_LOGI(TAG, "%s[DIR]  %s", indent, entry->d_name);

            // рекурсия
            list_dir(path, level + 1);
        }
        else
        {

            ESP_LOGI(TAG, "%s[FILE] %s (%ld bytes)",
                     indent, entry->d_name, st.st_size);
        }
    }

    closedir(dir);
}