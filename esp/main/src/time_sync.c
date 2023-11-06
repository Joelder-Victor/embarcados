#include "../include/time_sync.h"


const char *TAG_TIME = "TIME";

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG_TIME, "Notification of a time synchronization event");
}
void get_current_datetime(char * datetime)
{
    char strftime_buf[64];
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    // Set timezone to Indian Standard Time
    setenv("TZ", "BRT3", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    char formatted_datetime[30]; // Define um buffer para a string formatada

    // Formata a data e hora no formato desejado
    strftime(formatted_datetime, sizeof(formatted_datetime), "%Y-%m-%d %H:%M:%S", &timeinfo);
    // strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    // ESP_LOGI(TAG_TIME, "The current date/time in Delhi is: %s", formatted_datetime);
    strcpy(datetime, formatted_datetime);
    
}

void initialize_sntp(void)
{
    ESP_LOGI(TAG_TIME, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();
}
void obtain_time(void)
{

    initialize_sntp();
    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
    {
        //ESP_LOGI(TAG_TIME, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    time(&now);
    localtime_r(&now, &timeinfo);
}
void Set_SystemTime_SNTP()
{

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900))
    {
        ESP_LOGI(TAG_TIME, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        obtain_time();
        // update 'now' variable with current time
        time(&now);
    }
}
