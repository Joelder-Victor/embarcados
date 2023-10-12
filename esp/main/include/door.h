#ifndef DOOR_H
#define DOOR_H
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


#define OUT_SIGNAL GPIO_NUM_5
#define LEN_CACHE 20

void open_time(void);
int check_in(char *data);
static void IRAM_ATTR gpio_isr_handler(void *arg);
static void taskExitButton(void *arg);
void taskDoor(void *parameter);
void door(void);


#endif