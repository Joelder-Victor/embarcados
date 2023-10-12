#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H
#include "esp_http_client.h"

#define URL_SERVER "<<server>>/check_key"

esp_err_t client_event_post_handler(esp_http_client_event_handle_t evt);
int post_rest_function(char * post_data);

#endif