#include <stdio.h>
#include <string.h>
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define TAG "ESP_NOW_RECEIVER"

uint8_t sender_mac[] = {0x7C, 0x87, 0xCE, 0xF8, 0xC9, 0x59};

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
 * Funkcija za inicijalizaciju WiFi-ja
 * 
 * Potrebna jer ESPNOW protokol koristi WiFi pa je potrebno inicijalizirati 
 * hardver zasluzen za WiFi
 * 
 * return:      void
 */
void init_wifi() {
    esp_netif_init();
    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_APSTA);
    esp_wifi_start();
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
    memcpy(peerInfo.peer_addr, sender_mac, 6);
    peerInfo.channel = 0; 
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
    // nvs potreban za WiFi
    nvs_flash_init();
    init_wifi();
    init_espnow();

    // testiranje ESPNOW konekcije
    /* char message[] = "Poruka iz recievera";
    
    esp_err_t result = esp_now_send(receiver_mac, 
        (uint8_t *)message, 
        sizeof(message));

    if (result == ESP_OK) {
        ESP_LOGI(TAG, "Message sent successfully");
    } else {
        ESP_LOGE(TAG, "ESP-NOW send failed: %s", esp_err_to_name(result));
    } */

    ESP_LOGI(TAG, "ESP-NOW Receiver Ready!");
}