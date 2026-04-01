idf_component_register(
    SRCS 
        "MPU6050.c"
        "mpu6050_test.c"



    INCLUDE_DIRS
        "include" 


    REQUIRES 
        # nvs_flash
        # esp_wifi 
        # #mbedtls
        # wt_libs
        #esp_driver_i2c
        mpu6050
        # esp_driver_spi
        # esp_driver_gptimer
        # esp_driver_gpio
        # esp_driver_rmt
        # esp_driver_ledc
        # esp_driver_uart
        #AL
        config
        i2c
        webserver
        
)
   
