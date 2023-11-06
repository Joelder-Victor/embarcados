#ifndef DOOR_H
#define DOOR_H
#include <time.h>
#include "stdio.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "sdkconfig.h"

#define OUT_SIGNAL GPIO_NUM_5
#define LEN_CACHE 20
#define DOOR_ID CONFIG_DOOR_ID
#define SECRET_KEY  CONFIG_SECRET_KEY

void door(void);
void open_time(void);
void set_locale(void);
void print_timestamp(void); 
void taskDoor(void *parameter);
static void taskExitButton(void *arg);
int check_in(char *data, char * post_data);
static void IRAM_ATTR gpio_isr_handler(void *arg);


#endif