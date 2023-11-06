/* Door

*/

#include "include/ble.h"
#include "include/wifi.h"
#include "include/time_sync.h"
#include "include/http_client.h"


void app_main(void)
{
    door();
    connect_ble();
    wifi_connection();
    nvs_flash_init();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    printf("WIFI was initiated ...........\n");
    wake_up_server();
    Set_SystemTime_SNTP();
}
