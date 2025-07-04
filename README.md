# ESP_NOW_test

Proyecto experimental para establecer comunicación inalámbrica eficiente entre un dispositivo **Master ESP32** y múltiples **Slaves ESP32** utilizando el protocolo **ESP-NOW**, con confirmación de recepción mediante mensajes `ACK`.

---

## 📡 Descripción general

Este proyecto permite al Master enviar comandos a múltiples Slaves para controlar:

- 💡 Luz ambiental (PWM con niveles BAJO, MEDIO, ALTO)
- 🧼 Luz de desinfección (PWM)
- 🌬️ Sistema de purificación de aire (ventilador, ozonizador y lámpara UV)

* Cada Slave recibe los comandos y responde con un mensaje de ACK indicando recepción correcta.
* El sistema está preparado para escalar hasta **15 Slaves**, con identificación por SLAVE_ID y MAC.

---

## 🏗️ Arquitectura
```text
+---------------------+              +---------------------+
|        MASTER       |    ESP-NOW   |       SLAVE  N      |
|    (controlador)    |<------------>|    (actuadores)     |
+---------------------+              +---------------------+

```
---
## 📂 Estructura del proyecto

```text
├── Master
│   ├── lib
│   │   ├── EspNowInterface
│   │   │   ├── EspNowInterface.cpp
│   │   │   └── EspNowInterface.h
│   │   ├── MsgProtocol
│   │   │   ├── MsgProtocol.cpp
│   │   │   └── MsgProtocol.h
│   │   └── PeerTable
│   │       └── Peers.h
│   ├── platformio.ini
│   └── src
│       └── main.cpp
├── README.md
├── Slave1
│   ├── lib
│   │   ├── EspNowInterface
│   │   │   ├── EspNowInterface.cpp
│   │   │   └── EspNowInterface.h
│   │   ├── MsgProtocol
│   │   │   ├── MsgProtocol.cpp
│   │   │   └── MsgProtocol.h
│   │   ├── SlaveConfig
│   │   │   └── SlaveConfig.h
│   │   └── SlaveController
│   │       ├── SlaveController.cpp
│   │       └── SlaveController.h
│   ├── platformio.ini
│   └── src
│       └── main.cpp
└── Slave2
    ├── lib
    │   ├── EspNowInterface
    │   │   ├── EspNowInterface.cpp
    │   │   └── EspNowInterface.h
    │   ├── MsgProtocol
    │   │   ├── MsgProtocol.cpp
    │   │   └── MsgProtocol.h
    │   ├── SlaveConfig
    │   │   └── SlaveConfig.h
    │   └── SlaveController
    │       ├── SlaveController.cpp
    │       └── SlaveController.h
    ├── platformio.ini
    └── src
        └── main.cpp
```

---

## 🔐 Estructura de mensajes

### 🔸 `MsgToSlave`

```cpp
struct MsgToSlave {
    uint8_t payload_id;
    PWMLevel luz_ambiental;
    PWMLevel luz_desinfeccion;
    bool purificador_on;
};
```
### 🔸 `AckToMaster`
```cpp
struct AckToMaster {
    uint8_t payload_id;
    bool ok;
};
```
---
## 🧪 Pruebas realizadas

* Comunicación unicast del Master a múltiples Slaves

* ACK enviado por Slaves y recibido correctamente

* Retries automáticos si el ACK no llega (hasta 3 intentos)

* Mensajes con estados codificados con enums (PWMLevel)

* Rechazo de Slaves no registrados (por MAC)

---
## ⚙️ Compilación y uso - Requisitos

* ESP32 (cualquier modelo)
* PlatformIO + VSCode
* Cable USB para programación
---
## 📋 Configuración de MACs

En lib/PeerTable/Peers.h (Master):
```cpp
// Reemplazar con las MAC reales una vez conocidas
const SlaveEntry SLAVE_LIST[MAX_SLAVES] = {
    { 1, {0x10, 0x06, 0x1C, 0xF4, 0xC2, 0xC0} },
    { 2, {0x10, 0x06, 0x1C, 0xF4, 0x5A, 0xBC} },
};
```
En src/main.cpp (Slave1, Slave2,... SlaveN):
```cpp
// MAC del Master (a reemplazar con la real)
uint8_t master_mac[6] = {0x10, 0x06, 0x1C, 0xF4, 0xE8, 0x78};
};
```
`Estas MAC deben coincidir con las físicas de tus dispositivos.`

#### En lib/Slave1/SlaveConfig.h   
#### ó  
#### En lib/Slave2/SlaveConfig.h  

```cpp
#pragma once

constexpr uint8_t SLAVE_ID = 1; // Cambiar para cada firmware del Slave

```


---
# 🧠 Detalles técnicos importantes

* ESP-NOW requiere que el receptor esté registrado como peer para enviar mensajes unicast.

* El Master espera un ACK por cada mensaje enviado. Si no lo recibe, reintenta hasta 3 veces.

* La comparación de MACs para asociación del ACK con un Slave se realiza con memcmp.

* El sistema es extensible y puede incorporar lógica de retry por Slave, notificación por consola, y control interactivo desde UART.
---
## 🥇 Estado actual del proyecto
```text
✅ Comunicación Master ↔ Slaves
✅ ACKs recibidos y procesados correctamente
✅ Sin retries innecesarios
```
---
## 🛠️ Próximo paso:

Modo interactivo desde consola para cambiar comandos manualmente
---

## 📖 Licencia

Este proyecto está licenciado bajo la licencia MIT.

## 👤 Autor

Es un ejercicio educativo desarrollado por José Faginas, usando el siguiente toolchain: VsCode + PlatformIO en C++
