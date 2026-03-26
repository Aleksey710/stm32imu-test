cmake_minimum_required(VERSION 3.5)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Папки
set(FRONTEND_DIR ${CMAKE_SOURCE_DIR}/frontend)
set(GZ_DIR ${CMAKE_BINARY_DIR}/frontend_gz)

# 1. Копируем frontend → gz (с сохранением структуры)
add_custom_command(
    COMMENT "Copying frontend to gz directory"
    OUTPUT ${GZ_DIR}/.prepared
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${GZ_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GZ_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${FRONTEND_DIR} ${GZ_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${GZ_DIR}/.prepared
)

# 2. Сжимаем нужные файлы (рекурсивно)
add_custom_command(
    COMMENT "Compressing frontend files"
    OUTPUT ${GZ_DIR}/.compressed
    # gzip оставляет оригиналы благодаря -k:
    # index.html
    # index.html.gz
    # это важно — fallback если клиент не поддерживает gzip
    COMMAND find ${GZ_DIR} -name "*.html" -exec gzip -kf {} \\\;
    COMMAND find ${GZ_DIR} -name "*.js"   -exec gzip -kf {} \\\;
    COMMAND find ${GZ_DIR} -name "*.css"  -exec gzip -kf {} \\\;
    #COMMAND find ${GZ_DIR} -type f \\\( -name "*.html" -o -name "*.js" -o -name "*.css" \\\) -exec gzip -kf {} \\\;
    COMMAND ${CMAKE_COMMAND} -E touch ${GZ_DIR}/.compressed
    DEPENDS ${GZ_DIR}/.prepared
)

# 3. Создаём target
add_custom_target(compress_frontend ALL
    DEPENDS ${GZ_DIR}/.compressed
)

# 4. SPIFFS образ из gz папки
# storage - имя раздела в partitions.csv
#spiffs_create_partition_image(storage ${GZ_DIR} FLASH_IN_PROJECT)
#spiffs_create_partition_image(spiffs ${GZ_DIR} FLASH_IN_PROJECT)
# littlefs_create_partition_image(storage ${GZ_DIR} FLASH_IN_PROJECT
# DEPENDS compress_frontend
# COMMENT "littlefs_create_partition_image")

# 5. Удаляем gz после сборки
add_custom_command(
    COMMENT "Cleaning up gz directory"
    TARGET compress_frontend POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${GZ_DIR}
)

# set(
#     EXTRA_COMPONENT_DIRS 
#     ${CMAKE_SOURCE_DIR}/components/drivers
# )
# set(
#     EXTRA_COMPONENT_DIRS 
#     ${CMAKE_SOURCE_DIR}/components/imu
# )

project(esp32_imu)
