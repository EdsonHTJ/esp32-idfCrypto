#include "tron_http.h"

static const char *TAG = "http";

extern const char server_root_cert_pem_start[] asm("_binary_rootca_pem_start");
extern const char server_root_cert_pem_end[]   asm("_binary_rootca_pem_end");

#define ACCOUNT "TWsFJR5PPBa96PkNAPzKB6aLtvKpiP31na"
#define PATH "/v1/accounts/"

char output_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0x00};



__attribute__((weak)) void on_response_result_callback(char* p_data)
{

} 

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
     // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            memcpy(output_buffer + output_len, evt->data, evt->data_len);
            output_len += evt->data_len;
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            ESP_LOGD(TAG, "CONTENT %s", output_buffer);
            on_response_result_callback(output_buffer);
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

void https_with_hostname_path()
{
    esp_http_client_config_t config = {
        .host = "api.trongrid.io",
        .path = PATH ACCOUNT,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .event_handler = _http_event_handler,
        .cert_pem = server_root_cert_pem_start,
        .keep_alive_enable = true,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGD(TAG, "HTTP HEAD Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGD(TAG, "HTTP HEAD request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}
