#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H
#include "esp_http_client.h"

#define URL_SERVER "https://laser-1.onrender.com/esp/"
#define LEN_URL 50
#define LEN_POST_DATA 100
esp_err_t client_event_post_handler(esp_http_client_event_handle_t evt);
int post_rest_function(char * post_data, char *route);
void wake_up_server();
esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt);
//void insert_log(char * post_data);
#endif