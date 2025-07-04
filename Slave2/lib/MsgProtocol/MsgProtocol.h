#pragma once

#include <stdint.h>

// Nivel de PWM (común a luz ambiental y luz de desinfección)
enum class PWMLevel : uint8_t {
    OFF  = 0,
    BAJA = 1,
    MEDIA = 2,
    ALTA = 3
};

// Mensaje del Master al Slave
struct MsgToSlave {
    uint8_t payload_id;             // Identificador del mensaje
    PWMLevel luz_ambiental;         // PWM: OFF / BAJA / MEDIA / ALTA
    PWMLevel luz_desinfeccion;      // PWM: OFF / BAJA / MEDIA / ALTA
    bool purificador_on;            // true = ON, false = OFF
};

// ACK simple del Slave al Master
struct AckToMaster {
    uint8_t payload_id;             // Para validar que es respuesta a X mensaje
    bool ok;                        // true si la orden fue aceptada
};

// Funciones auxiliares para convertir enums a texto (opcional, útil para debug)
const char* pwmLevelToString(PWMLevel level);

