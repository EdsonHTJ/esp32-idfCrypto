#ifndef __TRON_HTTP__
#define __TRON_HTTP__

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "esp_tls.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"

#define MAX_HTTP_OUTPUT_BUFFER 8192

esp_http_client_handle_t https_init();
void https_with_hostname_path(esp_http_client_handle_t client, char* p_acc);

#endif