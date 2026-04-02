idf_component_register(
    SRCS 
        "i2c.c"
        "i2c_devices.c"



    INCLUDE_DIRS
        "include" 


    REQUIRES 
        driver
        #esp_driver_i2c
        
        # esp_driver_spi
        # esp_driver_gptimer
        # esp_driver_gpio
        # esp_driver_rmt
        # esp_driver_ledc
        # esp_driver_uart
        #AL
        config
        data

        MPU6050
)
   
