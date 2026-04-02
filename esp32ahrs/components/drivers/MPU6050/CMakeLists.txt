idf_component_register(
    SRCS 
        "MPU6050.c"
        "mpu6050_test.c"

    INCLUDE_DIRS
        "include" 

    REQUIRES 
        freertos
        driver

        config
        data
        i2c
        mpu6050

)
   
