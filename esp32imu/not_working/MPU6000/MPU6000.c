#include "i2c_mpu6000.h"
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO 9
#define I2C_MASTER_SDA_IO 8
#define I2C_PORT I2C_NUM_0

#define MPU_ADDR 0x68

static void i2c_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };
    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);
}

static void mpu_write(uint8_t reg, uint8_t val)
{
    uint8_t data[2] = {reg, val};
    i2c_master_write_to_device(I2C_PORT, MPU_ADDR, data, 2, 1000 / portTICK_PERIOD_MS);
}

static void mpu_read(uint8_t reg, uint8_t *buf, size_t len)
{
    i2c_master_write_read_device(I2C_PORT, MPU_ADDR, &reg, 1, buf, len, 1000 / portTICK_PERIOD_MS);
}

static void mpu_init()
{
    mpu_write(0x6B, 0x00); // wake up
}

void i2c_task(void *pv)
{
    QueueHandle_t queue = (QueueHandle_t)pv;
    uint8_t buf[14];
    mpu_data_t data;

    i2c_init();
    mpu_init();

    while (1) {
        mpu_read(0x3B, buf, 14);

        data.ax = (buf[0] << 8) | buf[1];
        data.ay = (buf[2] << 8) | buf[3];
        data.az = (buf[4] << 8) | buf[5];

        data.gx = (buf[8] << 8) | buf[9];
        data.gy = (buf[10] << 8) | buf[11];
        data.gz = (buf[12] << 8) | buf[13];

        xQueueSend(queue, &data, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
