#ifndef CONFIG_I2C_H
#define CONFIG_I2C_H
//----------------------------------------------------------------------
#include <inttypes.h>
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// I2C CONFIG
//----------------------------------------------------------------------
// #define I2C_PORT I2C_NUM_0
// #define I2C_SDA 21
// #define I2C_SCL 22
// #define I2C_FREQ 400000

#define I2C_PORT 0

// Пины Назначение
// GPIO 0–14 камера DVP
// GPIO 15–17 PSRAM/boot
// GPIO 18–21 иногда SDMMC/USB
// GPIO 43–44 USB/debug

// 👉 остаются “окна” типа 4,5, 8, 9
// ❌ Не использовать :

// GPIO 0(boot mode)
// GPIO 45–46(input - only / boot strap)
// пины камеры(Y2–Y9, XCLK, PCLK и т.д.)
// USB D+/D- (43/44 если используется USB)

#define I2C_SDA_GPIO 4
#define I2C_SCL_GPIO 5

#define I2C_FREQ 400000

#define MAX_DEVICES 8
//----------------------------------------------------------------------
#endif // CONFIG_I2C_H
