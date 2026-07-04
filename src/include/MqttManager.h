#pragma once

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <iostream>
#include <nlohmann/json.hpp>

#include "mqtt/async_client.h"

// Déclaration du template primaire MqttSerializer.
// Les spécialisations concrètes (JSON, etc.) sont fournies dans MqttJsonSerializer.h.
template <typename T>
struct MqttSerializer;

// On hérite publiquement de mqtt::callback
class MqttManager : public virtual mqtt::callback {
public:
    MqttManager(std::string serverAddress, std::string clientId);
    ~MqttManager() override;

    bool connecter();
    void deconnecter();

	// Pour des données simples (string), on peut utiliser ces deux fonctions directement
    bool souscrire(const std::string& topic, std::function<void(std::string, std::string)> messageCallback);
    bool publier(const std::string& topic, const std::string& payload);

	//pour des structures complexes (JSON), on peut utiliser ces deux fonctions génériques
    template <typename T>
    bool publierStructure(const std::string& topic, const T& donnee);

    template <typename T>
    bool souscrireStructure(const std::string& topic, std::function<void(std::string, T)> callbackStructure);

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

// L'astuce est ici : on inclut l'implémentation cachée à la fin du header
template <typename T>
bool MqttManager::publierStructure(const std::string& topic, const T& donnee) {
    try {
        nlohmann::json j = donnee;
        return publier(topic, j.dump());
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur de sérialisation JSON: " << e.what() << std::endl;
        return false;
    }
}

template <typename T>
bool MqttManager::souscrireStructure(const std::string& topic, std::function<void(std::string, T)> callbackStructure) {
    return souscrire(topic, [callbackStructure](std::string t, std::string payload) {
        try {
            nlohmann::json j = nlohmann::json::parse(payload);
            T donnee = j.get<T>();
            callbackStructure(t, donnee);
        }
        catch (const std::exception& e) {
            std::cerr << "Erreur de décodage JSON sur " << t << " : " << e.what() << std::endl;
        }
        });
}