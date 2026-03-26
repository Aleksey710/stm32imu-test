#include "driver/spi_master.h"
#include "esp_log.h"

static const char *TAG = "MPU6000";

#define PIN_NUM_MISO 13
#define PIN_NUM_MOSI 11
#define PIN_NUM_CLK  12
#define PIN_NUM_CS   10

static spi_device_handle_t spi;

static void spi_init()
{
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
    };

    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 3,
    };

    spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
}

static void mpu_write(uint8_t reg, uint8_t val)
{
    uint8_t data[2] = {reg & 0x7F, val};
    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = data,
    };
    spi_device_transmit(spi, &t);
}

static void mpu_read(uint8_t reg, uint8_t *buf, size_t len)
{
    uint8_t tx[1] = {reg | 0x80};

    spi_transaction_t t = {
        .length = (len + 1) * 8,
        .tx_buffer = tx,
        .rx_buffer = buf,
    };
    spi_device_transmit(spi, &t);
}
