#include "../include/http_client.h"

// Define client certificate
extern const uint8_t ClientCert_pem_start[] asm("_binary_certificate_pem_start");
extern const uint8_t ClientCert_pem_end[]   asm("_binary_certificate_pem_end");


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

int post_rest_function(char * post_data, char *route)
{ 
    char url_[LEN_URL]={URL_SERVER};
    //strcat(url_,URL_SERVER);
    strcat(url_,route);
    printf("post: %s\n",post_data);
    printf("url: %s\n",url_);

    esp_http_client_config_t config_post = {
        .url = url_,
        .method = HTTP_METHOD_POST,
        .cert_pem = (const char *)ClientCert_pem_start,
        .event_handler = client_event_post_handler,
        .timeout_ms = 60000};

    esp_http_client_handle_t client = esp_http_client_init(&config_post);

    
    printf("%s\n",post_data);
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_http_client_set_header(client, "accept", "application/json");
    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_err_t err = esp_http_client_perform(client);
    int response_code = 0;
    if (err == ESP_OK) {
        response_code = esp_http_client_get_status_code(client);
        printf("Response Code: %d\n", response_code);
    }
    else {
        response_code = -1;
    }
        
    esp_http_client_cleanup(client);

    return response_code;
}

esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
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

void wake_up_server()
{
    esp_http_client_config_t config_get = {
        .url = "https://laser-1.onrender.com",
        .method = HTTP_METHOD_GET,
        .cert_pem = (const char *)ClientCert_pem_start,
        .event_handler = client_event_get_handler,
        .timeout_ms = 60000};
        
    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}
