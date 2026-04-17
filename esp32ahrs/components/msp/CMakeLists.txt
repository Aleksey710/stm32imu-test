idf_component_register(
    SRCS 
        "msp.c"



    INCLUDE_DIRS
        "include" 


    REQUIRES 
		nvs_flash
		esp_event
		#esp_partition
		#spi_flash
		# esp_wifi 
		# #mbedtls
		# wt_libs
		esp_ringbuf
		# esp_driver_i2c
		# esp_driver_spi
		# esp_driver_gptimer
		# esp_driver_gpio
		# esp_driver_rmt
		# esp_driver_ledc
		# esp_driver_uart
		config
		data
		bluetooth


)
   
