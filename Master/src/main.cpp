#include <Arduino.h>
#include "EspNowInterface.h"
#include "Peers.h"
#include "MsgProtocol.h"

// Máximo intentos por Slave
constexpr int MAX_RETRIES = 3;
constexpr unsigned long ACK_TIMEOUT_MS = 200;

// Estructura para manejar estado de ACKs
struct AckStatus {
    bool received = false;
    bool success = false;
};

// Tabla de ACKs en espera (indexada por SLAVE_ID - 1)
AckStatus ack_table[MAX_SLAVES];

// Función callback de recepción
void onReceive(const uint8_t* mac, const uint8_t* data, int len) {
    if (len != sizeof(AckToMaster)) {
        Serial.println("[MASTER] ACK recibido con tamaño inválido");
        return;
    }

    AckToMaster ack;
    memcpy(&ack, data, sizeof(ack));

    // Buscar qué slave lo mandó
    for (int i = 0; i < MAX_SLAVES; ++i) {
        const SlaveEntry& slave = SLAVE_LIST[i];
        if (memcmp(mac, slave.mac, 6) == 0) {
            ack_table[slave.id - 1].received = true;
            ack_table[slave.id - 1].success = ack.ok;
            Serial.printf("[MASTER] ACK de SLAVE %u recibido: %s\n", slave.id, ack.ok ? "OK" : "ERROR");
            break;
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("[MASTER] Iniciando...");

    // // Set device as a Wi-Fi Station
    // WiFi.mode(WIFI_STA);
    // WiFi.disconnect();

    if (!EspNowInterface::begin()) {
        Serial.println("[ERROR] No se pudo iniciar ESP-NOW");
        return;
    }

    EspNowInterface::onReceive(onReceive);

    // Agregamos solo los 2 primeros Slaves
    for (int i = 0; i < 2; ++i) {
        const uint8_t* mac = SLAVE_LIST[i].mac;
        if (EspNowInterface::addPeer(mac)) {
            Serial.printf("[MASTER] Peer SLAVE %u agregado\n", SLAVE_LIST[i].id);
        } else {
            Serial.printf("[ERROR] No se pudo agregar SLAVE %u\n", SLAVE_LIST[i].id);
        }
    }

    delay(1000); // Esperamos un poco antes de enviar
}

void loop() {
    static uint8_t payload_id = 1;

    MsgToSlave msg = {
        .payload_id = payload_id++,
        .luz_ambiental = PWMLevel::BAJA,
        .luz_desinfeccion = PWMLevel::MEDIA,
        .purificador_on = true
    };

    // Enviar a cada Slave
    for (int i = 0; i < 2; ++i) {
        const SlaveEntry& slave = SLAVE_LIST[i];
        ack_table[slave.id - 1] = {};  // Reiniciar estado

        int retries = 0;
        bool success = false;

        while (retries < MAX_RETRIES && !ack_table[slave.id - 1].received) {
            Serial.printf("[MASTER] Enviando a SLAVE %u (intento %d)...\n", slave.id, retries + 1);
            bool sent = EspNowInterface::send(slave.mac, reinterpret_cast<uint8_t*>(&msg), sizeof(msg));

            if (!sent) {
                Serial.println("[ERROR] Falla al enviar");
                break;
            }

            // Esperar ACK
            unsigned long start = millis();
            while (millis() - start < ACK_TIMEOUT_MS) {
                if (ack_table[slave.id - 1].received) break;
                delay(10);
            }

            if (ack_table[slave.id - 1].received) {
                success = ack_table[slave.id - 1].success;
                break;
            }

            retries++;
        }

        if (!ack_table[slave.id - 1].received) {
            Serial.printf("[MASTER] Sin ACK de SLAVE %u\n", slave.id);
        } else if (!ack_table[slave.id - 1].success) {
            Serial.printf("[MASTER] ACK con error de SLAVE %u\n", slave.id);
        }
    }

    Serial.println("---- Ciclo completo ----");
    delay(5000); // Esperamos 5s y repetimos
}