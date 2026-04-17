idf_component_register(
    SRCS 
        "bitarray.c"
        "circular_queue.c"
        "crc.c"
        #"gps_conversion.c"
        #"maths.c"
        "streambuf.c"
        #"encoding.c"
        "string_light.c"
        #"typeconversion.c"
        "uvarint.c"


    INCLUDE_DIRS
		# для доступности файлов, прописанных в стиле 
	    # #include "common/maths.h"
        ".." 
		"." 


    REQUIRES 
		nvs_flash
		esp_event
		#esp_partition
		#spi_flash
		# esp_wifi 
		# #mbedtls
		# wt_libs
		#esp_ringbuf
		# esp_driver_i2c
		# esp_driver_spi
		# esp_driver_gptimer
		# esp_driver_gpio
		# esp_driver_rmt
		# esp_driver_ledc
		# esp_driver_uart

)
   
