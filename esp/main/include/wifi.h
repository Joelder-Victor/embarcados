#ifndef WIFI_H
#define WIFI_H

#include "esp_wifi.h"
#include "esp_netif.h"



#include "lwip/err.h" //light weight ip packets error handling
#include "lwip/sys.h" //system applications for light weight ip apps

#define SSID "brisa-2116012"
#define PASS ""

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void wifi_connection();

#endif