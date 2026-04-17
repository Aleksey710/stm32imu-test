#include "bluetooth.h"

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "host/ble_hs.h"
#include "host/util/util.h"

#include "config_bluetooth.h"
#include "ble_data.h"

//----------------------------------------------------------------------
static const char *TAG = "BLE";

// UUID NUS (Nordic UART)
static const ble_uuid128_t service_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
                     0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e);

static const ble_uuid128_t tx_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
                     0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e);

static uint16_t conn_handle;
static uint16_t tx_handle;
//----------------------------------------------------------------------
// 📡 отправка notify
void ble_send(uint8_t *data, uint16_t len)
{
    struct os_mbuf *om = ble_hs_mbuf_from_flat(data, len);
    ble_gattc_notify_custom(conn_handle, tx_handle, om);
}
//----------------------------------------------------------------------
// 📡 GATT
static int gatt_svc_access(uint16_t conn, uint16_t attr,
                           struct ble_gatt_access_ctxt *ctxt,
                           void *arg)
{
    return 0;
}
//----------------------------------------------------------------------
static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &service_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){
            {
                .uuid = &tx_uuid.u,
                .flags = BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &tx_handle,
                .access_cb = gatt_svc_access,
            },
            {0}},
    },
    {0},
};
//----------------------------------------------------------------------
// 📡 GAP
static int gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {

    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0)
        {
            conn_handle = event->connect.conn_handle;
            ESP_LOGI(TAG, "Connected");
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "Disconnected");
        ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER,
                          NULL, gap_event, NULL);
        break;
    }
    return 0;
}
//----------------------------------------------------------------------
// 📡 реклама
//----------------------------------------------------------------------
static void ble_app_on_sync(void)
{
    ble_svc_gap_device_name_set(BLE_DEVICE_NAME);

    struct ble_hs_adv_fields fields = {0};

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    const char *name = BLE_DEVICE_NAME;
    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;

    ble_gap_adv_set_fields(&fields);

    struct ble_gap_adv_params adv_params = {0};
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    adv_params.itvl_min = 0x20;
    adv_params.itvl_max = 0x40;

    int rc = ble_gap_adv_start(
        BLE_OWN_ADDR_PUBLIC,
        NULL,
        BLE_HS_FOREVER,
        &adv_params,
        gap_event,
        NULL);

    ESP_LOGI("BLE", "adv start rc=%d", rc);
}
//----------------------------------------------------------------------
// 📡 задача отправки из очереди
void ble_tx_task(void *arg)
{
    char data[128];

    while (1)
    {
        if (xQueueReceive(ble_queue, &data, portMAX_DELAY))
        {
            ble_send((uint8_t *)data, strlen(data));
        }
    }
}
//----------------------------------------------------------------------
// 📡 NimBLE loop
void ble_host_task(void *param)
{
    nimble_port_run();
}
//----------------------------------------------------------------------
void ble_init(const int cpuId)
{
    // nvs_flash_init();

    nimble_port_init();

    ble_svc_gap_init();
    ble_svc_gatt_init();

    // ble_svc_gap_device_name_set("ESP32-IMU");

    ble_gatts_count_cfg(gatt_svcs);
    ble_gatts_add_svcs(gatt_svcs);

    ble_hs_cfg.sync_cb = ble_app_on_sync;

    nimble_port_freertos_init(ble_host_task);

    //------------------------------------------------------------------
    ble_queue = xQueueCreate(10, 128);

    xTaskCreatePinnedToCore(ble_tx_task, "ble_tx", 4096, NULL, 5, NULL, cpuId);
}
