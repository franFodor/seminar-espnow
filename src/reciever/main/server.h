#pragma once

#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_now.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"

#define TAG "ESP_NOW_SENDER"

static const uint8_t receiver_mac[] = {0x34, 0x86, 0x5D, 0xDC, 0x20, 0xD0};

esp_err_t get_handler(httpd_req_t *req);
httpd_handle_t start_server();