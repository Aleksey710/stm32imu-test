idf_component_register(
	SRCS 
		"main.c"

    INCLUDE_DIRS 
		"."

    REQUIRES 	
		#esp_littlefs
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
