#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>

#include <MqttJsonSerializer.h>

enum class DeviceType {
    GPU,
    CPU,
    RAM
};

// Structure locale et pour l'ajout
struct Device {
    int id = -1; // -1 signifie non encore attribué par le Monitoring
    std::string nom;
    DeviceType type;
    int temperature;
    int charge;
};

// Structures spécifiques pour les payloads MQTT
struct MsgAjout {
    std::string nom;
    DeviceType type;
    int temperature;
    int charge;
};

// to_json/from_json générés automatiquement par nlohmann (macro intrusive)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MsgAjout, nom, type, temperature, charge)
MQTT_JSON_SERIALIZER(MsgAjout)

struct MsgSuppression {
    int id;
};

// to_json/from_json générés automatiquement par nlohmann (macro intrusive)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MsgSuppression, id)
MQTT_JSON_SERIALIZER(MsgSuppression)

struct MsgDataInt {
    int id;
    int data;
};

// to_json/from_json générés automatiquement par nlohmann (macro intrusive)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MsgDataInt, id, data)
MQTT_JSON_SERIALIZER(MsgDataInt)


#endif // STRUCTURES_H