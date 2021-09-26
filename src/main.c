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

QueueHandle_t xBuffQueue, xBalanceQueue;

void https_request_task(void *pvParameters){

    esp_http_client_handle_t client = https_init();
    for(;;)
    {
        https_with_hostname_path(client);
        vTaskDelay(20000 / portTICK_PERIOD_MS);
    }
    esp_http_client_cleanup(client);
}

void process_json_task(void *pvParameters){

    uint32_t data_ptr = 0x00;
    char* p_data;
    for(;;){
        xQueueReceive(xBuffQueue, (void*) &data_ptr, portMAX_DELAY);
        ESP_LOGI("main", "CONTENT PTR: %i", data_ptr);
        p_data = (char*) data_ptr;
        ESP_LOGI("main", "CONTENT: %s", p_data);

        cJSON* request_json = NULL;
        cJSON* data = cJSON_CreateArray();
        cJSON* balance = NULL;
        cJSON* subitem = NULL;

        request_json = cJSON_Parse(p_data);
        data = cJSON_GetObjectItem(request_json, "data");
        subitem = cJSON_GetArrayItem(data, 0);
        balance = cJSON_GetObjectItem(subitem, "balance");

        ESP_LOGI("main", "BALANCE: %i", balance->valueint);

        

    }
}

void balance_controller_task(void *pvParameters){
    int balance = -1;
    for(;;){

    }
}

void on_response_result_callback(char* p_data, size_t size)
{

    
    ESP_LOGI("main", "Content: %s", p_data);

    char* ptr = strstr(p_data, "\"balance\"");
    ptr += sizeof("\"balance\"");
    char* ptrEnd = strstr(ptr, ",");
    size_t strSize = (size_t)(ptrEnd - ptr);
    ESP_LOGI("main", "size: %i", strSize);

    char balanceStr[strSize + 1];
    memset(balanceStr, 0x00, sizeof(balanceStr));
    strncpy(balanceStr, ptr, sizeof(balanceStr));

    balanceStr[strSize] = 0x00;
    long balance = atol(balanceStr);
   
    ESP_LOGI("main", "B: %ld", balance);

} 

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    xBuffQueue = xQueueCreate(2, sizeof(char*));
    xBalanceQueue = xQueueCreate(2, sizeof(int));

    xTaskCreate(&https_request_task, "https_request", 5 * 8192, NULL, 5, NULL);
    xTaskCreate(&process_json_task, "process_json", 8192, NULL, 5, NULL);
}
