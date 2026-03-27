#ifndef CONFIG_WIFI_H
#define CONFIG_WIFI_H
//----------------------------------------------------------------------
#include <inttypes.h>
#include "config_wifi_custom.h"
//----------------------------------------------------------------------
// WiFi

#ifdef WIFI_SSID_CUSTOM
#define WIFI_SSID WIFI_SSID_CUSTOM
#else
#define WIFI_SSID ("SSID")
#endif

#ifdef WIFI_PASSWORD_CUSTOM
#define WIFI_PASSWORD WIFI_PASSWORD_CUSTOM
#else
#define WIFI_PASSWORD ("PASSWORD")
#endif

//----------------------------------------------------------------------
#endif // CONFIG_WIFI_H