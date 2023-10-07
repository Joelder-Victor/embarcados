/* Door

*/
#include <stdio.h>
// #include <cJSON.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_http_client.h"


#include "lwip/err.h" //light weight ip packets error handling
#include "lwip/sys.h" //system applications for light weight ip apps

#define SSID "brisa-2116012"
#define PASS ""

#define DOOR_NAME "SALA 1"
#define OUT_SIGNAL GPIO_NUM_5
#define PASSWORD "SALA1"
static TaskHandle_t taskDoor_handle = NULL;

static QueueHandle_t gpio_isr_queue = NULL;

int button_pin = 2;

char *TAG = DOOR_NAME;
uint8_t ble_addr_type;
void ble_app_advertise(void);

void open_time(void)
{
    vTaskDelay(10000 / portTICK_PERIOD_MS);
}
//------------------------------BLE-------------------------------------------------//

int check_in(char *data)
{

    // cJSON *root = cJSON_Parse(data);

    if (data == NULL)
    {
        printf("Error parsing JSON.\n");
        return 0;
    }
    char id[7];
    strncpy(id, data, 6);
    id[6] = '\0';

    char key[13];
    strncpy(key, data + 6, 12);
    key[18] = '\0';

    // cJSON *id = cJSON_GetObjectItem(root, "id");
    // cJSON *key = cJSON_GetObjectItem(root, "key");

    // if (id != NULL && key != NULL && cJSON_IsString(id) && cJSON_IsString(key)) {
    printf("id: %s\n", id);
    printf("key: %s\n", key);
    // } else{
    //     printf("Invalid JSON format.\n");
    //     return 0;
    // }

    if (strcmp(key, PASSWORD) == 0)
        return 1;
    return 0;
}
// Write data to ESP32 defined as server
static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    printf("Data from the client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);

    char *data = (char *)ctxt->om->om_data;
    printf("Length Data %d\n", strlen(data));

    if (check_in(data))
    {
        gpio_set_level(OUT_SIGNAL, 0);
        open_time();
        gpio_set_level(OUT_SIGNAL, 1);
        memset(data, 0, strlen(data));
        return 0;
    }

    memset(data, 0, strlen(data));

    return 1;

    // return 0;
}

// Read data from ESP32 defined as server
/*static int device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    os_mbuf_append(ctxt->om, "Data from the server", strlen("Data from the server"));
    return 0;
}
*/
// Array of pointers to other service definitions
// UUID - Universal Unique Identifier
static const struct ble_gatt_svc_def gatt_svcs[] = {
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(0x180), // Define UUID for device type
     .characteristics = (struct ble_gatt_chr_def[]){
         /*{.uuid = BLE_UUID16_DECLARE(0xFEF4), // Define UUID for reading
          .flags = BLE_GATT_CHR_F_READ,
          .access_cb = device_read},*/
         {.uuid = BLE_UUID16_DECLARE(0xDEAD), // Define UUID for writing
          .flags = BLE_GATT_CHR_F_WRITE,
          .access_cb = device_write},
         {0}}},
    {0}};

// BLE event handling
static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    // Advertise if connected
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        if (event->connect.status != 0)
        {
            ble_app_advertise();
        }
        break;
    // Advertise again after completion of the event
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT DISCONNECTED");
        if (event->connect.status != 0)
        {
            ble_app_advertise();
        }
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("GAP", "BLE GAP EVENT");
        ble_app_advertise();
        break;
    default:
        break;
    }
    return 0;
}

// Define the BLE connection
void ble_app_advertise(void)
{
    // GAP - device name definition
    struct ble_hs_adv_fields fields;
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    device_name = ble_svc_gap_device_name(); // Read the BLE device name
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    // GAP - device connectivity definition
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; // connectable or non-connectable
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN; // discoverable or non-discoverable
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

// The application
void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type); // Determines the best address type automatically
    ble_app_advertise();                     // Define the BLE connection
}

// The infinite task
void host_task(void *param)
{
    nimble_port_run(); // This function will return only when nimble_port_stop() is executed
}

void connect_ble()
{

    nvs_flash_init(); // 1 - Initialize NVS flash using
    // esp_nimble_hci_and_controller_init();      // 2 - Initialize ESP controller
    nimble_port_init();                     // 3 - Initialize the host stack
    ble_svc_gap_device_name_set(DOOR_NAME); // 4 - Initialize NimBLE configuration - server name
    ble_svc_gap_init();                     // 4 - Initialize NimBLE configuration - gap service
    ble_svc_gatt_init();                    // 4 - Initialize NimBLE configuration - gatt service
    ble_gatts_count_cfg(gatt_svcs);         // 4 - Initialize NimBLE configuration - config gatt services
    ble_gatts_add_svcs(gatt_svcs);          // 4 - Initialize NimBLE configuration - queues gatt services.
    ble_hs_cfg.sync_cb = ble_app_on_sync;   // 5 - Initialize application
    nimble_port_freertos_init(host_task);   // 6 - Run the thread
}

//-----------------------------------BUTTON------------------------------//
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    xQueueSendFromISR(gpio_isr_queue, arg, NULL);
}

static void taskExitButton(void *arg)
{
    int pino = 0;
    int isClosed = 1;
    while (1)
    {
        if (xQueueReceive(gpio_isr_queue, &pino, portMAX_DELAY))
        {
            printf("Opening the door from inside\n");
            gpio_set_level(OUT_SIGNAL, !isClosed);
            open_time();
            gpio_set_level(OUT_SIGNAL, isClosed);
        }
    }
}
void taskDoor(void *parameter)
{
    int isClosed = 1;
    if (gpio_get_level(OUT_SIGNAL) == 0)
    {
        gpio_set_level(OUT_SIGNAL, isClosed);
    }
    while (1)
    {
        if (gpio_get_level(OUT_SIGNAL) == 0)
        {
            printf("The door is open\n");
        }
        else
        {
            printf("The door is closed\n");
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
void door(void)
{

    gpio_isr_queue = xQueueCreate(1, sizeof(int));

    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_POSEDGE | GPIO_INTR_NEGEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL << 2;
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << 5);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(GPIO_NUM_2, gpio_isr_handler, &button_pin);

    xTaskCreate(taskDoor, "Task door", 2048, NULL, 0, &taskDoor_handle);
    xTaskCreate(taskExitButton, "Interrupt task", 2048, NULL, 24, NULL);
}
//------------------------------------- HTTP CLIENT--------------------------------------------//
int retry_num=0;
int got_ip = 0;
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_STA_START)
    {
        printf("WIFI CONNECTING....\n");
    }
    else if (event_id == WIFI_EVENT_STA_CONNECTED)
    {
        printf("WiFi CONNECTED\n");
    }
    else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        printf("WiFi lost connection\n");
        if (retry_num < 5)
        {
            esp_wifi_connect();
            retry_num++;
            printf("Retrying to Connect...\n");
        }
    }
    else if (event_id == IP_EVENT_STA_GOT_IP)
    {
        printf("Wifi got IP...\n\n");
        got_ip =1;
    }
}

void wifi_connection()
{
     //                          s1.4
    // 2 - Wi-Fi Configuration Phase
    esp_netif_init();
    esp_event_loop_create_default();     // event loop                    s1.2
    esp_netif_create_default_wifi_sta(); // WiFi station                      s1.3
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); //     
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS,
            
           }
    
        };
    strcpy((char*)wifi_configuration.sta.ssid, SSID);
    strcpy((char*)wifi_configuration.sta.password, PASS);    
    //esp_log_write(ESP_LOG_INFO, "Kconfig", "SSID=%s, PASS=%s", ssid, pass);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
    printf( "wifi_init_softap finished. SSID:%s  password:%s",SSID,PASS);
    
}

esp_err_t client_event_post_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}

static void post_rest_function()
{
    esp_http_client_config_t config_post = {
        .url = "http://ip:8000/check_key",
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL,
        .event_handler = client_event_post_handler};

    esp_http_client_handle_t client = esp_http_client_init(&config_post);

    char *post_data = "{\"key\": \"5b798cd1a3f8\"}";
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

void app_main(void)
{
    door();
    connect_ble();
    nvs_flash_init();
    while (!got_ip)
    {
    
        wifi_connection();

        vTaskDelay(5000 / portTICK_PERIOD_MS);
        printf("WIFI was initiated ...........\n\n");
    }
    
    post_rest_function();
}
