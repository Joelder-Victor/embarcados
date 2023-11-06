#include <time.h>
#include <sys/time.h>
#include "esp_sntp.h"
#include "esp_log.h"
void initialize_sntp(void);
static void obtain_time(void);
void Set_SystemTime_SNTP(void);
void get_current_datetime(char * datetime);
void time_sync_notification_cb(struct timeval *tv);