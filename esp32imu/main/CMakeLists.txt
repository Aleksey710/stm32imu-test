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
		i2c

		MPU6050

)
#-----------------------------------------------------------------------
# Сборка и подготовка frontend (клиентская часть сайта)
#-----------------------------------------------------------------------
message(STATUS "UNIX = ${UNIX}")
message(STATUS "WIN32 = ${WIN32}")
message(STATUS "APPLE = ${APPLE}")
message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_HOST_SYSTEM_NAME = ${CMAKE_HOST_SYSTEM_NAME}")

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Linux detected")
	message(STATUS "Сборка web компонента (сайта)")

	# Каталог хранилища
	set(STORAGE_DIR ${CMAKE_BINARY_DIR}/storage)
	# Исходники web
	set(FRONTEND_DIR ${CMAKE_SOURCE_DIR}/web)
	# Каталог frontend в хранилище
	set(FRONTEND_STORAGE_DIR ${STORAGE_DIR}/frontend)


	message(STATUS "FRONTEND_DIR           = ${FRONTEND_DIR}")
	message(STATUS "FRONTEND_STORAGE_DIR   = ${FRONTEND_STORAGE_DIR}")

	# Очистка каталога хранилища сайта
	add_custom_command(
		COMMENT "Clear ${FRONTEND_STORAGE_DIR}"
		COMMAND ${CMAKE_COMMAND} -E rm -rf ${FRONTEND_STORAGE_DIR}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${FRONTEND_STORAGE_DIR}
		COMMAND ${CMAKE_COMMAND} -E touch ${FRONTEND_STORAGE_DIR}/.cleared
		OUTPUT ${FRONTEND_STORAGE_DIR}/.cleared
	)

	# Сборка сайта
	add_custom_command(
		COMMENT "Build web app"
		DEPENDS ${FRONTEND_STORAGE_DIR}/.cleared
		WORKING_DIRECTORY ${FRONTEND_DIR}
		# Нужна если пересобирать с нуля. 
		#COMMAND npm install
		COMMAND npm run build
		COMMAND ${CMAKE_COMMAND} -E touch ${FRONTEND_STORAGE_DIR}/.created
		OUTPUT ${FRONTEND_STORAGE_DIR}/.created
	)

	# Копируем frontend/dist → storage (с сохранением структуры)
	add_custom_command(
		COMMENT "Copying web/dist to storage directory"
		DEPENDS ${FRONTEND_STORAGE_DIR}/.created
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${FRONTEND_DIR}/dist ${FRONTEND_STORAGE_DIR}
		COMMAND ${CMAKE_COMMAND} -E touch ${FRONTEND_STORAGE_DIR}/.prepared
		OUTPUT ${FRONTEND_STORAGE_DIR}/.prepared
	)

	# Сжимаем нужные файлы (рекурсивно)
	# gzip оставляет оригиналы благодаря -k:
	# index.html
	# index.html.gz
	# это важно — fallback если клиент не поддерживает gzip
	# Только для пользователей Linux
	add_custom_command(
		COMMENT "Compressing frontend files"
		DEPENDS ${FRONTEND_STORAGE_DIR}/.prepared
		COMMAND find ${FRONTEND_STORAGE_DIR} -name "*.html" -exec gzip -kf {} \\\;
		COMMAND find ${FRONTEND_STORAGE_DIR} -name "*.js"   -exec gzip -kf {} \\\;
		COMMAND find ${FRONTEND_STORAGE_DIR} -name "*.css"  -exec gzip -kf {} \\\;
		COMMAND find ${FRONTEND_STORAGE_DIR} -name "*.ico"  -exec gzip -kf {} \\\;
		COMMAND find ${FRONTEND_STORAGE_DIR} -name "*.svg"  -exec gzip -kf {} \\\;
		COMMAND find ${FRONTEND_STORAGE_DIR} -name "*.jpg"  -exec gzip -kf {} \\\;
		COMMAND find ${FRONTEND_STORAGE_DIR} -name "*.jpeg" -exec gzip -kf {} \\\;
		COMMAND find ${FRONTEND_STORAGE_DIR} -name "*.png"  -exec gzip -kf {} \\\;
		COMMAND ${CMAKE_COMMAND} -E touch ${FRONTEND_STORAGE_DIR}/.compressed
		OUTPUT ${FRONTEND_STORAGE_DIR}/.compressed
	)

	# Создаём target (создание ${FRONTEND_STORAGE_DIR}/.compressed)
	# (дождаться окончания сжатия всех файлов)
	add_custom_target(
		compress_frontend ALL
		DEPENDS ${FRONTEND_STORAGE_DIR}/.compressed
	)

	# Создать образ раздела из подготовленного каталога с файлами
	# storage - имя раздела в partitions.csv
	#spiffs_create_partition_image(
	#    storage ${STORAGE_DIR} FLASH_IN_PROJECT
	#)
	littlefs_create_partition_image(
		storage ${STORAGE_DIR} FLASH_IN_PROJECT
	)

	# ВАЖНО: зависимость на partition, а не на компонент
	#add_dependencies(storage compress_frontend)
	add_dependencies(storage-flash compress_frontend)
	add_dependencies(littlefs_storage_bin compress_frontend)

	# 5. Target для удаления gz после сборки образа
	# add_custom_target(clean_frontend_gz
	#     COMMAND ${CMAKE_COMMAND} -E rm -rf ${FRONTEND_STORAGE_DIR}
	# )

	# # 6. Устанавливаем зависимость: удаление выполняется после создания образа
	# add_dependencies(clean_frontend_gz fs_image)
	# ${FRONTEND_STORAGE_DIR}/.cleared
	# ${FRONTEND_STORAGE_DIR}/.created
	# ${FRONTEND_STORAGE_DIR}/.prepared
	# ${FRONTEND_STORAGE_DIR}/.compressed

endif()

