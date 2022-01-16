#include "tron_http.h"

static const char *TAG = "http";

extern const char server_root_cert_pem_start[] asm("_binary_rootca_pem_start");
extern const char server_root_cert_pem_end[]   asm("_binary_rootca_pem_end");

#define ACCOUNT "TWsFJR5PPBa96PkNAPzKB6aLtvKpiP31na"
#define PATH "/v1/accounts/"
#define HOST "api.trongrid.io"
#define HTTPS_PREFIX "https://"
#define URL HTTPS_PREFIX HOST PATH

char output_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0x00};



__attribute__((weak)) void on_response_result_callback(char* p_data, size_t len)
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
            memset(output_buffer, 0x00, MAX_HTTP_OUTPUT_BUFFER);
            output_len = 0;
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
            ESP_LOGI(TAG, "CONTENT LEN %i", output_len);
            on_response_result_callback(output_buffer, output_len);
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

esp_http_client_handle_t https_init()
{
    esp_http_client_config_t config = {
        .url = HTTPS_PREFIX HOST,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .event_handler = _http_event_handler,
        .cert_pem = server_root_cert_pem_start,
        .timeout_ms = 2000
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    return client;
}

void https_with_hostname_path(esp_http_client_handle_t client, char* p_acc)
{
    char url[sizeof(URL) + 35] = URL;
    strncpy(url + sizeof(URL) - 1, p_acc, 34);
    ESP_LOGI(TAG, "url: %s", url);
    esp_http_client_set_url(client, url);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGD(TAG, "HTTP HEAD Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGD(TAG, "HTTP HEAD request failed: %s", esp_err_to_name(err));
    }
}
