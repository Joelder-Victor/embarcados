#include "../include/door.h"
#include "../include/http_client.h"
#include "../include/wifi.h"
static TaskHandle_t taskDoor_handle = NULL;

static QueueHandle_t gpio_isr_queue = NULL;

int button_pin = 2;
int count_keys = 1;
char keys[LEN_CACHE][18] = {SECRET_KEY};

void open_time(void)
{
    vTaskDelay(10000 / portTICK_PERIOD_MS);
}
void set_locale(void){
   setenv("LC_ALL", "pt_BR.UTF-8", 1); 
   tzset();
}
void print_timestamp(void){
    set_locale();
    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);

    printf("Data e Hora Atuais: %s\n", asctime(&timeinfo));
}
int check_in(char *data, char * post_data)
{
    int response = 0;
    if (data == NULL)
    {
        return 0;
    }
    printf("Verificando\n");
    for (size_t i = 0; i < LEN_CACHE; i++)
    {
        if (strcmp(data, keys[i]) == 0)
        {
            printf("OPEN!\n");
            //memset(data, 0, strlen(data));
            return 1;
        }
    }
    if (get_status())
    {
       
        int status_code = post_rest_function(post_data,"check_key");

        if (status_code == 200){
            response = 1;
            if (count_keys == LEN_CACHE)
                count_keys = 1;
            
            strcpy(keys[count_keys],data);
            count_keys++;
        }
        else if (status_code == 404)
            printf("Invalid Key.");
        else
            printf("Erro no servidor.");

        //memset(data, 0, strlen(data));
        //memset(post_data, 0, strlen(post_data));
        
    }
    return response;
}

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
