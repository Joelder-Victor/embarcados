/* Door

*/

#include "include/ble.h"
#include "include/wifi.h"
#include "include/http_client.h"

void app_main(void)
{
    door();
    connect_ble();
    nvs_flash_init();
    wifi_connection();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    printf("WIFI was initiated ...........\n");
    //vTaskDelay(5000000 / portTICK_PERIOD_MS);
}
