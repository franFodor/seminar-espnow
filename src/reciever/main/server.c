#include "server.h"

esp_err_t get_handler(httpd_req_t *req) {
    char query[128];
    char message[64] = {0};

    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
        ESP_LOGI(TAG, "Received Query: %s", query);
        if (httpd_query_key_value(query, "message", message, 
            sizeof(message)) == ESP_OK) {
            ESP_LOGI(TAG, "Extracted Message: %s", message);

            // slanje poruke esp now protokolom
            esp_err_t result = esp_now_send(receiver_mac, 
                (uint8_t *)message, strlen(message));
            if (result == ESP_OK) {
                httpd_resp_send(req, "ESP-NOW Message Sent!", 
                    HTTPD_RESP_USE_STRLEN);
                return ESP_OK;
            } else {
                httpd_resp_send(req, "ESP-NOW Message Failed!", 
                    HTTPD_RESP_USE_STRLEN);
                return ESP_OK;
            }
        }
    }

    // html stranica
    const char *html_response =
        "<html>"
        "<head><title>ESP32 Sender</title></head>"
        "<body>"
        "<h2>Send a Message via ESP-NOW</h2>"
        "<form action='/send' method='get'>"
        "  <input type='text' name='message' placeholder='Poruka' required>"
        "  <button type='submit'>Send</button>"
        "</form>"
        "</body>"
        "</html>";

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_response, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Start HTTP Server
httpd_handle_t start_server() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t uri_get = {
            .uri       = "/send",
            .method    = HTTP_GET,
            .handler   = get_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &uri_get);
    }
    return server;
}
