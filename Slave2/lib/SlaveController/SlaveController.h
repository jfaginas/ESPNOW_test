#pragma once

#include <Arduino.h>
#include "MsgProtocol.h"

class SlaveController {
public:
    static void begin();                      // Inicializa los GPIO/PWM
    static void applyCommand(const MsgToSlave& msg); // Aplica un mensaje
};

