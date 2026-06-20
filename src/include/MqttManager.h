#pragma once

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <iostream>
#include "mqtt/async_client.h"

// On hérite publiquement de mqtt::callback
class MqttManager : public virtual mqtt::callback {
public:
    MqttManager(std::string serverAddress, std::string clientId);
    ~MqttManager() override;

    bool connecter();
    void deconnecter();
    bool souscrire(const std::string& topic, std::function<void(std::string, std::string)> messageCallback);
    bool publier(const std::string& topic, const std::string& payload);

    // On surcharge la fonction de Paho pour intercepter TOUS les messages
    void message_arrived(mqtt::const_message_ptr msg) override;

    // Optionnel mais recommandé par Paho C++:
    void connection_lost(const std::string& cause) override {}
    void delivery_complete(mqtt::delivery_token_ptr tok) override {}

private:
    std::string m_serverAddress;
    std::string m_clientId;
    std::unique_ptr<mqtt::async_client> m_client;

    // Le dictionnaire qui associe un Topic à sa fonction de rappel
    std::map<std::string, std::function<void(std::string, std::string)>> m_callbacks;
};