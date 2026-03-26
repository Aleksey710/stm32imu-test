idf_component_register(
    SRCS 
        "webserver.c"



    INCLUDE_DIRS
        "include" 


    REQUIRES 
        esp_wifi 
        nvs_flash
        esp_http_server 
        esp_littlefs
        

        fs
        config
        cjson
        # mqtt
        # nvs_flash
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
)
   
