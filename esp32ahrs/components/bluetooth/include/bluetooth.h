#ifndef BLUETOOTHE_H
#define BLUETOOTHE_H
//----------------------------------------------------------------------
#include <inttypes.h>
//----------------------------------------------------------------------
void ble_init(const int cpuId);

// 📡 отправка notify
void ble_send(uint8_t *data, uint16_t len);
//----------------------------------------------------------------------
#endif // BLUETOOTHE_H
