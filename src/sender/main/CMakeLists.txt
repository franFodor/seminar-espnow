idf_component_register(SRCS "server.c" "sender.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi esp_netif esp_event nvs_flash esp_http_server)
