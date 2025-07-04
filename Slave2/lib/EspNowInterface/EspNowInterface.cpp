#include "EspNowInterface.h"

static ReceiveCallback userReceiveCallback = nullptr;
static SentCallback userSentCallback = nullptr;

static void onReceiveWrapper(const uint8_t* mac, const uint8_t* data, int len) {
    if (userReceiveCallback) {
        userReceiveCallback(mac, data, len);
    }
}

static void onSentWrapper(const uint8_t* mac, esp_now_send_status_t status) {
    if (userSentCallback) {
        userSentCallback(mac, status);
    }
}

// bool EspNowInterface::begin(uint8_t wifi_channel) {
//     WiFi.mode(WIFI_STA);
//     WiFi.disconnect();

//     // if (!esp_wifi_set_promiscuous(true)) return false;
//     // if (!esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE)) return false;
//     // if (!esp_wifi_set_promiscuous(false)) return false;

//     if (esp_now_init() != ESP_OK) {
//         return false;
//     }

//     esp_now_register_recv_cb(onReceiveWrapper);
//     esp_now_register_send_cb(onSentWrapper);
//     return true;
// }

// bool EspNowInterface::begin(uint8_t wifi_channel) {
//     WiFi.mode(WIFI_STA);              // Modo estación (requerido por ESP-NOW)
//     WiFi.disconnect(true);            // Desconectamos de cualquier AP anterior
//     delay(100);                       // Espera breve para evitar race conditions

//     // Seteamos el canal WiFi de forma segura (sin promiscuous mode)
//     esp_err_t err = esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);
//     if (err != ESP_OK) {
//         Serial.printf("[ERROR] esp_wifi_set_channel falló: %d\n", err);
//         return false;
//     }

//     // Inicializamos ESP-NOW
//     if (esp_now_init() != ESP_OK) {
//         Serial.println("[ERROR] esp_now_init() falló");
//         return false;
//     }

//     // Registramos los callbacks
//     esp_now_register_recv_cb(onReceiveWrapper);
//     esp_now_register_send_cb(onSentWrapper);

//     return true;
// }
// bool EspNowInterface::begin(uint8_t wifi_channel) {
//     WiFi.mode(WIFI_STA);
//     WiFi.disconnect(true);  // Asegura estado limpio

//     // Asegura que el driver WiFi esté arrancado antes de usar funciones de bajo nivel
//     if (esp_wifi_start() != ESP_OK) {
//         Serial.println("[ERROR] esp_wifi_start() falló");
//         return false;
//     }

//     delay(100); // Estabiliza el driver

//     // Setea el canal de manera segura
//     esp_err_t err = esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);
//     if (err != ESP_OK) {
//         Serial.printf("[ERROR] esp_wifi_set_channel falló: %d\n", err);
//         return false;
//     }

//     if (esp_now_init() != ESP_OK) {
//         Serial.println("[ERROR] esp_now_init() falló");
//         return false;
//     }

//     esp_now_register_recv_cb(onReceiveWrapper);
//     esp_now_register_send_cb(onSentWrapper);

//     return true;
// }


bool EspNowInterface::begin(uint8_t wifi_channel) {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);  // debe venir después de disconnect()
    delay(100);           // estabiliza el driver

    // Seteamos canal (debería funcionar si WiFi está en modo STA)
    esp_err_t err = esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);
    if (err != ESP_OK) {
        Serial.printf("[ERROR] esp_wifi_set_channel falló: %d\n", err);
        return false;
    }

    if (esp_now_init() != ESP_OK) {
        Serial.println("[ERROR] esp_now_init() falló");
        return false;
    }

    esp_now_register_recv_cb(onReceiveWrapper);
    esp_now_register_send_cb(onSentWrapper);

    return true;
}


bool EspNowInterface::addPeer(const uint8_t* mac, uint8_t channel) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = channel;
    peerInfo.encrypt = false;

    if (esp_now_is_peer_exist(mac)) {
        return true;  // Ya agregado
    }

    return esp_now_add_peer(&peerInfo) == ESP_OK;
}

bool EspNowInterface::send(const uint8_t* mac, const uint8_t* data, size_t len) {
    return esp_now_send(mac, data, len) == ESP_OK;
}

void EspNowInterface::onReceive(ReceiveCallback callback) {
    userReceiveCallback = callback;
}

void EspNowInterface::onSent(SentCallback callback) {
    userSentCallback = callback;
}

