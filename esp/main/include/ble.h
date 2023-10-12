#ifndef BLE_H
#define BLE_H

#include "door.h"

#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "nvs_flash.h"


#define DOOR_NAME "SALA 1"




static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

static int ble_gap_event(struct ble_gap_event *event, void *arg);

void ble_app_advertise(void);
void ble_app_on_sync(void);
void host_task(void *param);
void connect_ble();
char * save_data(char * key);
#endif