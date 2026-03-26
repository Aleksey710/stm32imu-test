Решение: включить WebSocket в menuconfig

Открой конфиг:

idf.py menuconfig

Дальше:

Component config
  → HTTP Server
     → [ ] WebSocket support   ← ВКЛЮЧИТЬ

Поставь:

[*] WebSocket support
//----------------------------------------------------------------------
Включи в menuconfig:
idf.py menuconfig

Дальше:

Component config → SPIFFS → Enable SPIFFS
//----------------------------------------------------------------------


