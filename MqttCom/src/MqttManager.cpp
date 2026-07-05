#include "MqttManager.h"
#include "mqtt/async_client.h" // Inclus ici uniquement, invisible pour l'extérieur
#include <iostream>

// Constructeur : On instancie dynamiquement le client Paho
MqttManager::MqttManager(std::string serverAddress, std::string clientId)
    : m_serverAddress(std::move(serverAddress)), m_clientId(std::move(clientId))
{
    m_client = std::make_unique<mqtt::async_client>(m_serverAddress, m_clientId);

    // On lie le manager au client Paho
    m_client->set_callback(*this);
}

// Destructeur : On coupe la connexion proprement si elle est active (principe du RAII)
MqttManager::~MqttManager() {
    deconnecter();
}

bool MqttManager::connecter() {
    try {
        mqtt::connect_options connOpts;
        connOpts.set_clean_session(true); // Session propre au démarrage
        connOpts.set_automatic_reconnect(true); // Reconnexion automatique en cas de coupure réseau

        std::cout << "[MqttManager] Connexion au broker " << m_serverAddress << "...\n";
        
        // .wait() transforme l'appel asynchrone en appel bloquant pour simplifier l'initialisation
        m_client->connect(connOpts)->wait(); 
        
        std::cout << "[MqttManager] Connecte avec succes !\n";
        return true;
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "[MqttManager] Erreur de connexion : " << exc.what() << "\n";
        return false;
    }
}

void MqttManager::deconnecter() {
    try {
        if (m_client && m_client->is_connected()) {
            m_client->disconnect()->wait();
            std::cout << "[MqttManager] Deconnecte du broker.\n";
        }
    }
    catch (const mqtt::exception& exc) {
        // Le bloc n'est plus vide, clang-tidy est content, et tu as l'info !
        std::cerr << "[MqttManager] Erreur lors de la deconnexion: " << exc.what() << "\n";
    }
}

bool MqttManager::publier(const std::string& topic, const std::string& payload) {
    if (!m_client || !m_client->is_connected()) {
        std::cerr << "[MqttManager] Erreur : impossible de publier, client non connecté.\n";
        return false;
    }
    
    try {
        // Envoi du message (QoS 1, Non-Retained)
        m_client->publish(topic, payload, 1, false)->wait();
        return true;
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "[MqttManager] Erreur d'envoi sur " << topic << " : " << exc.what() << "\n";
        return false;
    }
}

// On enregistre la fonction dans notre std::map et on demande l'abonnement au broker.
bool MqttManager::souscrire(const std::string& topic, std::function<void(std::string, std::string)> messageCallback) {
    if (!m_client || !m_client->is_connected()) {
        return false;
    }

    try {
        // 1. On s'abonne au topic sur le broker
        m_client->subscribe(topic, 1)->wait();

        // 2. On stocke proprement le callback dans notre map (avec un std::move efficace)
        m_callbacks[topic] = std::move(messageCallback);

        std::cout << "[MqttManager] Abonne au topic : " << topic << "\n";
        return true;
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "[MqttManager] Erreur d'abonnement : " << exc.what() << "\n";
        return false;
    }
}

// Dès que le broker envoie un message, cette fonction est déclenchée.
// Elle regarde de quel topic il s'agit, cherche le bon callback dans la map, et l'exécute.
void MqttManager::message_arrived(mqtt::const_message_ptr msg) {
    if (!msg) return;

    std::string topic = msg->get_topic();

    // On cherche si on a un callback enregistré pour ce topic précis
    auto it = m_callbacks.find(topic);
    if (it != m_callbacks.end()) {
        // On exécute la fonction associée en lui passant le topic et le message
        it->second(topic, msg->get_payload_str());
    }
    else {
        std::cout << "[MqttManager] Message reçu sur " << topic << " mais aucun callback associé.\n";
    }
}

