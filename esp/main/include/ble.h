#ifndef BLE_H
#define BLE_H

#include "door.h"
#include "time_sync.h"
#include "http_client.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "nvs_flash.h"

#define DOOR_NAME "SALA 2"
#define LEN_POST_DATA 100
#define LEN_KEY 18
#define LEN_DATETIME 30
void connect_ble();
void ble_app_on_sync(void);
void host_task(void *param);
void ble_app_advertise(void);
static int ble_gap_event(struct ble_gap_event *event, void *arg);
void create_post_data(char *post_data, const char *key, const char *hour);
static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
static int device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
#endif