#include "../include/ble.h"

char *TAG = DOOR_NAME;
uint8_t ble_addr_type;

char current_datetime[LEN_DATETIME];
int count_log = 0;

struct Log
{
    char key[LEN_KEY];
    char hour[LEN_DATETIME];
};

struct Log logging[LEN_CACHE];

// UUID - Universal Unique Identifier
static const struct ble_gatt_svc_def gatt_svcs[] = {
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(0x1802), // Define UUID for device type
     .characteristics = (struct ble_gatt_chr_def[]){
         {.uuid = BLE_UUID16_DECLARE(0xFEF4), // Define UUID for reading
          .flags = BLE_GATT_CHR_F_READ,
          .access_cb = device_read},
         {.uuid = BLE_UUID16_DECLARE(0x2a01), // Define UUID for writing
          .flags = BLE_GATT_CHR_F_WRITE,
          .access_cb = device_write},
         {0}}},
    {0}};

// Read data from ESP32 defined as server
static int device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    os_mbuf_append(ctxt->om, "Data from the server", strlen("Data from the server"));
    return 0;
}
void create_post_data(char *post_data, const char *key, const char *hour)
{

    if (strlen(hour))
    {
        snprintf(post_data, LEN_POST_DATA, "{\"key\":\"%s\",\"door_id\":\"%s\",\"hour\":\"%s\"}", key, DOOR_ID, hour);
        return;
    }
    snprintf(post_data, LEN_POST_DATA, "{\"key\":\"%s\",\"door_id\":\"%s\"}", key, DOOR_ID);
    return;
}

static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    int response = 1;

    printf("Data from the client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);

    char *data = (char *)ctxt->om->om_data;
    char post_data[LEN_POST_DATA];

    create_post_data(&post_data, data, "");
    printf("Length Data %d\n", strlen(data));

    if (check_in(data, post_data))
    {
        gpio_set_level(OUT_SIGNAL, 0);
        open_time();
        gpio_set_level(OUT_SIGNAL, 1);

        get_current_datetime(current_datetime);

        create_post_data(&post_data, data, current_datetime);

        response = post_rest_function(post_data, "log_entries");

        if (response == 200)
        {
            printf("Log inserted successfully!");
            if (count_log)
            {
                int temp = count_log;
                for (int i = count_log - 1; i >= 0; i--)
                {
                    create_post_data(&post_data, logging[i].key, logging[i].hour);
                    response = post_rest_function(post_data, "log_entries");
                    if (response == 200)
                    {
                        temp--;
                        memset(logging[i].key, 0, strlen(logging[i].key));
                        memset(logging[i].hour, 0, strlen(logging[i].hour));
                    }
                    printf("Valor de i %d\n", i);
                }
                count_log = temp;
            }
        }
        else
        {
            strcpy(logging[count_log].key, data);
            strcpy(logging[count_log].hour, current_datetime);
            printf("Log salvo em cache!");
            count_log++;
        }
        memset(data, 0, strlen(data));
        memset(post_data, 0, strlen(post_data));
        return 0;
    }

    memset(data, 0, strlen(data));

    return 1;
}
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
    ESP_LOGI("BLE", "SERVICE STARTED");
}
