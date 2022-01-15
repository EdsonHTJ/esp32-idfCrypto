/* HTTP GET Example using plain POSIX sockets

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "cJSON.h"
#include "driver/gpio.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "esp_tls.h"
#include "esp_crt_bundle.h"

#include "esp_http_client.h"
#include "tron_http.h"
#include "cJSON.h"

#define RELAY1 12
#define LED 2

#define ACCOUNT "TWsFJR5PPBa96PkNAPzKB6aLtvKpiP31na"

typedef enum gpio_enum{
    BLINK_BLUE_LED = 0,
    ACTIVATE_RELAY1,
} gpio_enum_t;

QueueHandle_t xBalanceQueue, xGpioQueue;

void https_request_task(void *pvParameters)
{
    esp_http_client_handle_t client = https_init();
    for(;;)
    {
        https_with_hostname_path(client, ACCOUNT);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    esp_http_client_cleanup(client);
}

void balance_controller_task(void *pvParameters)
{
    long balance = -1;
    long prevBalance = -1;
    for(;;){

        /*xQueueReceive(xGpioQueue, &command, portMAX_DELAY);
        ESP_LOGI("main", "COMMAND RECIVED %i", (int)command);*/
        prevBalance = balance;
        xQueueReceive(xBalanceQueue, &balance, portMAX_DELAY);
        ESP_LOGI("main", "Balance: %ld", balance);
        if(prevBalance == -1){
            continue;
        }

        if(balance > prevBalance){
            gpio_enum_t command = ACTIVATE_RELAY1;
            xQueueSend(xGpioQueue, &command, portMAX_DELAY);
        }

    }
}

void gpio_task(void *pvParameters)
{
    gpio_enum_t command = -1;
    uint32_t level = 1; 
    for(;;){

        xQueueReceive(xGpioQueue, &command, portMAX_DELAY);
        ESP_LOGI("main", "COMMAND RECIVED %i", (int)command);

        switch (command)
        {
        case BLINK_BLUE_LED:
            ESP_LOGI("main", "LED BLINK CALLED");
            level = 1;
            for(int i = 0; i < 10; i++){
                level = !level;
                gpio_set_level(LED, level);
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
            break;

        case ACTIVATE_RELAY1:
            ESP_LOGI("main", "RELAY CALLED");
            gpio_set_level(RELAY1, 1);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            gpio_set_level(RELAY1, 0);
            break;

        default:
            break;
        }

    }
}

void on_response_result_callback(char* p_data, size_t size)
{

    long balance;
    gpio_enum_t command = BLINK_BLUE_LED;
    xQueueSend(xGpioQueue, &command, portMAX_DELAY);
    
    ESP_LOGI("main", "Content: %s", p_data);

    char* ptr = strstr(p_data, "\"balance\"");
    if (ptr == NULL) {
        balance = 0;
        xQueueSend(xBalanceQueue, &balance, portMAX_DELAY);
        return;
    }
    
    ptr += sizeof("\"balance\"");
    char* ptrEnd = strstr(ptr, ",");
    size_t strSize = (size_t)(ptrEnd - ptr);
    ESP_LOGI("main", "size: %i", strSize);

    char balanceStr[strSize + 1];
    memset(balanceStr, 0x00, sizeof(balanceStr));
    strncpy(balanceStr, ptr, sizeof(balanceStr));

    balanceStr[strSize] = 0x00;
    balance = atol(balanceStr);
   
    ESP_LOGI("main", "B: %ld", balance);

    xQueueSend(xBalanceQueue, &balance, portMAX_DELAY);

} 

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    xBalanceQueue = xQueueCreate(2, sizeof(long));
    xGpioQueue = xQueueCreate(2, sizeof(gpio_enum_t));

    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(RELAY1);
    gpio_set_direction(RELAY1, GPIO_MODE_OUTPUT);

    xTaskCreate(&https_request_task, "https_request", 5 * 8192, NULL, 5, NULL);
    xTaskCreate(&balance_controller_task, "balance_controller", 8192, NULL, 5, NULL);
    xTaskCreate(&gpio_task, "gpio_task", 2048 , NULL, 5, NULL);
}
