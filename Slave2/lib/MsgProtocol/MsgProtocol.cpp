#include "MsgProtocol.h"

const char* pwmLevelToString(PWMLevel level) {
    switch (level) {
        case PWMLevel::OFF:  return "OFF";
        case PWMLevel::BAJA: return "BAJA";
        case PWMLevel::MEDIA: return "MEDIA";
        case PWMLevel::ALTA:  return "ALTA";
        default: return "UNKNOWN";
    }
}



