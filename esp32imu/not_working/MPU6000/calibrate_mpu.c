typedef struct {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
} mpu_data_t;

static mpu_data_t offset = {0};

void calibrate_mpu()
{
    ESP_LOGI(TAG, "Calibrating...");
    int samples = 100;

    for (int i = 0; i < samples; i++) {
        mpu_data_t d = read_raw();
        offset.ax += d.ax;
        offset.ay += d.ay;
        offset.az += d.az;
        offset.gx += d.gx;
        offset.gy += d.gy;
        offset.gz += d.gz;
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    offset.ax /= samples;
    offset.ay /= samples;
    offset.az /= samples;
    offset.gx /= samples;
    offset.gy /= samples;
    offset.gz /= samples;

    ESP_LOGI(TAG, "Calibration done");
}

//Применение:

//data.ax -= offset.ax;
