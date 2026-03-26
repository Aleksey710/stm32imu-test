# Проект предназначен для тестирования датчиков и фильтров для инерциальных навигационных систем

| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |


esp32imu - проект для ESP32
docs - проект документации
docker - развертывание документации


//----------------------------------------------------------------------
Решение: включить WebSocket в menuconfig

Открой конфиг:

idf.py menuconfig

Дальше:

Component config
  → HTTP Server
     → [ ] WebSocket support   ← ВКЛЮЧИТЬ

Поставь:

[*] WebSocket support

## Работа файловой системы

//----------------------------------------------------------------------
# SPIFFS
Включи в menuconfig:
idf.py menuconfig

Дальше:

Component config → SPIFFS → Enable SPIFFS
//----------------------------------------------------------------------
# LittleFS

## Добавление в проект

https://components.espressif.com/components/joltwallet/littlefs/versions/1.20.4/readme
git submodule add https://github.com/joltwallet/esp_littlefs.git
git submodule update --init --recursive

ESP-IDF при сборке LittleFS создает Python virtualenv (littlefs_py_venv) 
и в нем ставит утилиту littlefs-python для генерации образа. 
Если littlefs-python не установлен или виртуальное окружение не создано, CMake падает.


Проверяем Python
python3 --version
python3 -m venv --help

ESP-IDF требует Python ≥ 3.8.

Удаляем сломанное окружение
rm -rf build/littlefs_py_venv

Проверяем pip и setuptools
python3 -m pip install --upgrade pip setuptools wheel

Создаем виртуальное окружение вручную (опционально)
python3 -m venv build/littlefs_py_venv
source build/littlefs_py_venv/bin/activate
pip install littlefs-python

Собираем проект заново
idf.py fullclean
idf.py build

# где прописан FS_BASE_PATH

CONFIG_FS.h
fs.c
webserver.c
partitions_littlefs.csv
main/CMakeLists.txt (littlefs_create_partition_image(
    fs ${GZ_DIR} FLASH_IN_PROJECT
))


то это должно совпадать с:

.partition_label = "storage"

# где прописан partitions_littlefs.csv
partitions_littlefs.csv
sdkconfig
sdkconfig.defaults
sdkconfig.old



## Изменения

| Дата | Изменения |
| ----------- | --------- |
| 2026/03 |  - Начало проекта

