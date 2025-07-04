#include "SlaveController.h"

// Pines físicos (debes adaptarlos)
constexpr uint8_t PIN_LUZ_AMBIENTAL     = 18; // PWM
constexpr uint8_t PIN_LUZ_DESINFECCION  = 19; // PWM
constexpr uint8_t PIN_RELE_VENTILADOR   = 23; // ON/OFF
constexpr uint8_t PIN_RELE_OZONO        = 22;
constexpr uint8_t PIN_RELE_UV           = 21;

// Valores PWM (0-255) según nivel
static uint8_t pwmFromLevel(PWMLevel level) {
    switch (level) {
        case PWMLevel::OFF:  return 0;
        case PWMLevel::BAJA: return 85;   // ≈ 33%
        case PWMLevel::MEDIA:return 170;  // ≈ 66%
        case PWMLevel::ALTA: return 255;
        default: return 0;
    }
}

void SlaveController::begin() {
    pinMode(PIN_LUZ_AMBIENTAL, OUTPUT);
    pinMode(PIN_LUZ_DESINFECCION, OUTPUT);

    ledcSetup(0, 5000, 8);  // canal 0, freq 5kHz, resolución 8 bits
    ledcSetup(1, 5000, 8);

    ledcAttachPin(PIN_LUZ_AMBIENTAL, 0);
    ledcAttachPin(PIN_LUZ_DESINFECCION, 1);

    pinMode(PIN_RELE_VENTILADOR, OUTPUT);
    pinMode(PIN_RELE_OZONO, OUTPUT);
    pinMode(PIN_RELE_UV, OUTPUT);

    digitalWrite(PIN_RELE_VENTILADOR, LOW);
    digitalWrite(PIN_RELE_OZONO, LOW);
    digitalWrite(PIN_RELE_UV, LOW);
}

void SlaveController::applyCommand(const MsgToSlave& msg) {
    // PWM para luz ambiental y desinfección
    ledcWrite(0, pwmFromLevel(msg.luz_ambiental));
    ledcWrite(1, pwmFromLevel(msg.luz_desinfeccion));

    // Relés del purificador
    digitalWrite(PIN_RELE_VENTILADOR, msg.purificador_on ? HIGH : LOW);
    digitalWrite(PIN_RELE_OZONO,      msg.purificador_on ? HIGH : LOW);
    digitalWrite(PIN_RELE_UV,         msg.purificador_on ? HIGH : LOW);
}

