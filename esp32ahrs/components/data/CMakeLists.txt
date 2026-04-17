idf_component_register(
    SRCS 
        "gyroscope_data.c"
        "accelerometer_data.c"
        "IMU_data.c"
        "magnetometer_data.c"
        "AHRS_data.c"
        "ble_data.c"

        "ws_msg.c"
        "ws_msg_ringbuf.c"
        

    INCLUDE_DIRS
        "include" 


    REQUIRES 
        esp_ringbuf
        config
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
   
