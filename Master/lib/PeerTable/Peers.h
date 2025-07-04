#pragma once

#include <stdint.h>

constexpr int MAX_SLAVES = 2;

struct SlaveEntry
{
    uint8_t id;         // ID lógico del slave
    uint8_t mac[6];     // Dirección MAC del slave
};

// Reemplazar con las MAC reales una vez conocidas
const SlaveEntry SLAVE_LIST[MAX_SLAVES] = {
    { 1, {0x10, 0x06, 0x1C, 0xF4, 0xC2, 0xC0} },
    { 2, {0x10, 0x06, 0x1C, 0xF4, 0x5A, 0xBC} },
};

// Función auxiliar (opcional): buscar MAC por ID
inline const uint8_t* getMacById(uint8_t slave_id) {
    for (int i = 0; i < MAX_SLAVES; ++i) {
        if (SLAVE_LIST[i].id == slave_id) {
            return SLAVE_LIST[i].mac;
        }
    }
    return nullptr;
}

