#include "server.h"

/*
 * Callback funkcija za slanje podataka
 * 
 * mac_addr:    MAC adresa uredjaja sa kojeg saljemo podatke
 * status:      status poslane poruke
 * 
 * return:      void
 */
void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    ESP_LOGI(TAG, "ESP-NOW Send Status: %s", 
        (status == ESP_NOW_SEND_SUCCESS) ? "Success" : "Fail");
}

/*
 * Callback funkcija za slanje podataka
 * 
 * recv_info:   podatci o uredjaju sa kojeg smo primili poruku
 * data:        podatci koje primamo
 * len:         velicina polja data
 * 
 * return:      void
 */
void on_data_recv(const esp_now_recv_info_t *recv_info, 
    const uint8_t *data, int len) {
    //const uint8_t *mac_addr = recv_info->src_addr;

    //ESP_LOGI("ESP-NOW", "Received data from: %02X:%02X:%02X:%02X:%02X:%02X",
    //         mac_addr[0], mac_addr[1], mac_addr[2], 
    //         mac_addr[3], mac_addr[4], mac_addr[5]);

    ESP_LOGI("ESP-NOW", "Data: %.*s", len, data);
}

/*
 * Funkcija za inicijalizaciju WiFi-ja u AP nacinu rada kako bi se 
 * mogli povezati preko browsera i slati poruke
 * 
 * return:      void
 */
void init_wifi_ap() {
    esp_netif_init();
    esp_event_loop_create_default();

    esp_netif_t *esp_netif = esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_wifi_set_mode(WIFI_MODE_AP);
    wifi_config_t ap_config = {
        .ap = {
            .ssid = "ESP32_AP",
            .ssid_len = strlen("ESP32_AP"),
            .channel = 1,
            .password = "12345678",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };
    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    esp_wifi_start();

    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif, &ip_info);
    ESP_LOGI(TAG, "ESP32 AP IP Address: " IPSTR, IP2STR(&ip_info.ip));
}

/*
 * Funkcija za inicijalizaciju ESPNOW protokola
 * 
 * return:      void
 */
void init_espnow() {
    // inicijalizacija
    if (esp_now_init() != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW Init Failed");
        return;
    }

    // registriranje callback funckija za primanje i slanje podataka
    esp_now_register_send_cb(on_data_sent);
    esp_now_register_recv_cb(on_data_recv);

    // spajanje s drugim uredjajem
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, receiver_mac, 6);
    peerInfo.channel = 0;
    peerInfo.ifidx = ESP_IF_WIFI_AP;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add peer");
        return;
    }

    ESP_LOGI(TAG, "ESP-NOW Initialized Successfully");
}

/*
 * Glavna funkcija iz koje se pozivaju sve potrebne inicijalizacije
 * 
 * return:      void
 */
void app_main() {
    nvs_flash_init();
    init_wifi_ap();
    init_espnow();

    // testiranje ESPNOW konekcije
    // char message[] = "Hello from ESP-NOW Sender!";

    // esp_err_t result = esp_now_send(receiver_mac, 
    //     (uint8_t *)message, 
    //     sizeof(message));

    // if (result == ESP_OK) {
    //     ESP_LOGI(TAG, "Message sent successfully");
    // } else {
    //     ESP_LOGE(TAG, "ESP-NOW send failed: %s", esp_err_to_name(result));
    // }

    start_server();
    ESP_LOGI(TAG, "ESP-NOW Sender with Web Server Ready!");
}
