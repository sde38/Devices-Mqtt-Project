#pragma once

struct HardwareDevice {
    int id;                 // Requis par le concept HasIntegralId
    std::string name;       // Ex: "NVIDIA RTX 4090", "Intel Core i9"
    std::string type;       // Ex: "GPU", "CPU", "RAM"
    float temperature;      // Donnée dynamique (ex: 65.5°C)
    float loadPercentage;   // Pourcentage d'utilisation
};