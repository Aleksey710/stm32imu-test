idf_component_register(
	SRCS 
		"main.c"

    INCLUDE_DIRS 
		"."

    REQUIRES 	
		esp_littlefs
		#esp_partition
		#spi_flash
		# esp_wifi 
		# #mbedtls
		# wt_libs
		# esp_driver_i2c
		# esp_driver_spi
		# esp_driver_gptimer
		# esp_driver_gpio
		# esp_driver_rmt
		# esp_driver_ledc
		# esp_driver_uart
		config
		fs
		wifi
		webserver


)
#-----------------------------------------------------------------------
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
    COMMAND find ${GZ_DIR} -name "*.ico"  -exec gzip -kf {} \\\;
    COMMAND find ${GZ_DIR} -name "*.svg"  -exec gzip -kf {} \\\;
    COMMAND ${CMAKE_COMMAND} -E touch ${GZ_DIR}/.compressed
    DEPENDS ${GZ_DIR}/.prepared
)

# 3. Создаём target (создание ${GZ_DIR}/.compressed)
add_custom_target(
	compress_frontend ALL
    DEPENDS ${GZ_DIR}/.compressed
)

# 4. SPIFFS образ из gz папки
# storage - имя раздела в partitions.csv
#spiffs_create_partition_image(storage ${GZ_DIR} FLASH_IN_PROJECT)
littlefs_create_partition_image(
    fs ${GZ_DIR} FLASH_IN_PROJECT
)

# 5. Target для удаления gz после сборки образа
# add_custom_target(clean_frontend_gz
#     COMMAND ${CMAKE_COMMAND} -E rm -rf ${GZ_DIR}
# )

# # 6. Устанавливаем зависимость: удаление выполняется после создания образа
# add_dependencies(clean_frontend_gz fs_image)
