# Проект предназначен для тестирования датчиков и фильтров для инерциальных навигационных систем

| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |


esp32imu - проект для ESP32
docs - проект документации
docker - развертывание документации

## Заметки

Инфу с датчиков (на разных частотах измерения) пропускать через преобразование Фурье.
Системы на столе и системы на БПЛА. 
Для оценки частотной составляющей шума и выбора оптимальных методов фильтрации 

//----------------------------------------------------------------------
## Короткая шпаргалка CMake в ESP-IDF
Переменная | Значение
| ---------------------- | ---------------- |
${CMAKE_SOURCE_DIR} | корень проекта
${CMAKE_CURRENT_SOURCE_DIR} | текущий компонент
${CMAKE_BINARY_DIR} | build/
${PROJECT_DIR} | ESP-IDF root project

🧠 Почему так в ESP-IDF

littlefs_create_partition_image() генерирует:

Target                   | Назначение
| ---------------------- | ---------------- |
littlefs_storage_bin     | бинарник FS
storage-flash            | прошивка FS
encrypted-storage-flash	 | если encryption
//----------------------------------------------------------------------
## Работа с WebSocket
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
Задать размер Flash size. (Зависит от контроллера)

idf.py menuconfig

→ Serial flasher config → Flash size → 16MB
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

As a ESP-IDF managed component: In your project directory run
idf.py add-dependency joltwallet/littlefs==1.20.4

As a submodule: In your project, add this as a submodule to your components/ directory.
git submodule add https://github.com/joltwallet/esp_littlefs.git components/esp_littlefs
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


# где прописан FS_BASE_PATH
partitions.csv
main/CMakeLists.txt (littlefs_create_partition_image(
    fs ${GZ_DIR} FLASH_IN_PROJECT
))
то это должно совпадать с:
.partition_label = "storage"

# где прописан partitions.csv
partitions.csv
sdkconfig
sdkconfig.defaults
sdkconfig.old

## Driver for MPU6000/MPU6050 6-axis MotionTracking device.

https://components.espressif.com/components/esp-idf-lib/mpu6050/versions/2.1.3/readme?language=en

https://github.com/esp-idf-lib/mpu6050/blob/main/examples/default/README.md

#Installation
idf.py add-dependency esp-idf-lib/mpu6050


# альтернативная установка в git проект
git submodule add https://github.com/esp-idf-lib/mpu6050.git
git submodule update --init --recursive


## Изменения

| Дата | Изменения |
| ----------- | --------- |

| 2026/03/27 |  - работает webserver. Файлы из /storage [LittleFS]
| 2026/03/26 |  - Начало проекта

