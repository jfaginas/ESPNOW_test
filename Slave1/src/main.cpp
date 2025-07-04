#include <Arduino.h>
#include "MsgProtocol.h"
#include "EspNowInterface.h"
#include "SlaveConfig.h"
#include "SlaveController.h"

// MAC del Master (a reemplazar con la real)
uint8_t master_mac[6] = {0x10, 0x06, 0x1C, 0xF4, 0xE8, 0x78};

// Función de callback cuando llega un mensaje
void onReceive(const uint8_t* mac, const uint8_t* data, int len) {
    if (len != sizeof(MsgToSlave)) {
        Serial.println("[SLAVE] Mensaje recibido de tamaño inválido");
        return;
    }

    MsgToSlave msg;
    memcpy(&msg, data, sizeof(MsgToSlave));

    Serial.printf("[SLAVE %u] Recibido payload_id %u\n", SLAVE_ID, msg.payload_id);
    Serial.printf(" - Luz ambiental: %s\n", pwmLevelToString(msg.luz_ambiental));
    Serial.printf(" - Luz desinfección: %s\n", pwmLevelToString(msg.luz_desinfeccion));
    Serial.printf(" - Purificador: %s\n", msg.purificador_on ? "ON" : "OFF");

    // Aplicar orden al hardware
    SlaveController::applyCommand(msg);

    // Enviar ACK
    AckToMaster ack = {
        .payload_id = msg.payload_id,
        .ok = true
    };
    EspNowInterface::addPeer(master_mac);
    EspNowInterface::send(mac, reinterpret_cast<uint8_t*>(&ack), sizeof(ack));
    Serial.println("[SLAVE] ACK enviado");
}

void setup() {
    Serial.begin(115200);
    delay(500); // Espera para evitar conflictos al arrancar

    Serial.printf("[SLAVE %u] Inicializando...\n", SLAVE_ID);

    if (!EspNowInterface::begin()) {
        Serial.println("[ERROR] No se pudo iniciar ESP-NOW");
        return;
    }



    EspNowInterface::onReceive(onReceive);

    SlaveController::begin();

    Serial.println("[SLAVE] Listo para recibir comandos del master.");
}

void loop() {
    // Nada por ahora. Se ejecuta solo por callbacks.
}

