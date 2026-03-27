#ifndef CONFIG_FS_H
#define CONFIG_FS_H
//----------------------------------------------------------------------
#include <inttypes.h>
//----------------------------------------------------------------------
// максимальная длинна полного имени файла
#define FILEPATH_MAX 256

// путь, по которому файловая система будет доступна в коде.
// "/" нельзя
#define FS_BASE_PATH "/storage"

// каталог вебсервера = FS_BASE_PATH + "/folder_name"
#define FS_WEBSERVER_PATH "/storage/frontend"

// имя раздела во флешке, где лежит LittleFS.
// Из файла partitions.csv(таблица разделов)
#define FS_PARTITION_LABEL "storage"
//----------------------------------------------------------------------
#endif // CONFIG_FS_H
