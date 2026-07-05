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
    int id; // -1 signifie non encore attribué par le Monitoring
    std::string nom;
    DeviceType type;
    int temperature;
    int charge;
};

// Structures spécifiques pour les payloads MQTT
struct MsgAjoutDevice {
    int id;
    std::string nom;
    DeviceType type;
    int temperature;
    int charge;
};
// to_json/from_json générés automatiquement par nlohmann (macro intrusive)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MsgAjoutDevice, id, nom, type, temperature, charge)


struct MsgSuppression {
    int id;
};
// to_json/from_json générés automatiquement par nlohmann (macro intrusive)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MsgSuppression, id)


struct MsgDataInt {
    int id;
    int data;
};
// to_json/from_json générés automatiquement par nlohmann (macro intrusive)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MsgDataInt, id, data)


// Noms de topics
static constexpr const char* TOPIC_AJOUT        = "device/ajout";
static constexpr const char* TOPIC_SUPPRESSION  = "device/suppression";
static constexpr const char* TOPIC_SURVEILLANCE = "device/surveillance";
static constexpr const char* TOPIC_DEVICE_ID    = "device/id";
static constexpr const char* TOPIC_TEMPERATURE  = "device/temperature";
static constexpr const char* TOPIC_CHARGE       = "device/charge";

#endif // STRUCTURES_H