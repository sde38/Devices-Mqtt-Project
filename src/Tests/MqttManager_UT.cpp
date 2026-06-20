#include <gtest/gtest.h>
#include "MqttManager.h"
#include <chrono>
#include <thread>
#include <atomic>
#include <string>

// Configuration globale pour les tests
const std::string BROKER_ADDRESS = "tcp://localhost:1883";
const std::string TEST_TOPIC     = "test/mqttcomm/unittest";
const std::string TEST_PAYLOAD   = "Hello_MQTT_Unitaire";

// --- FIXTURE DE TEST ---
// Permet de partager la configuration et l'initialisation entre les tests
class MqttManagerTest : public ::testing::Test {
protected:
    // Cette fonction s'exécute AVANT chaque test
    void SetUp() override {
        mqttTest = std::make_unique<MqttManager>(BROKER_ADDRESS, "Client_UT_Docker");
        
        // Tentative de connexion initiale
        brokerDisponible = mqttTest->connecter();
        if (!brokerDisponible) {
            std::cerr << "[WARNING] Le broker MQTT n'est pas joignable sur " 
                      << BROKER_ADDRESS << ". Certains tests vont être ignorés.\n";
        }
    }

    // Cette fonction s'exécute APRÈS chaque test
    void TearDown() override {
        if (mqttTest && brokerDisponible) {
            mqttTest->deconnecter();
        }
        mqttTest.reset();
    }

    std::unique_ptr<MqttManager> mqttTest;
    bool brokerDisponible = false;
};

// --- TESTS UNITAIRES & INTÉGRATION ---

// 1. Test de la connexion
TEST_F(MqttManagerTest, TestConnexionBroker) {
    // Si le broker n'est pas disponible, on force l'échec explicite avec un message clair
    ASSERT_TRUE(brokerDisponible) << "Impossible de se connecter au broker à l'adresse : " << BROKER_ADDRESS;
}

// 2. Test complet de la chaîne : Souscription, Publication et Réception
TEST_F(MqttManagerTest, TestSouscriptionPublicationEtReception) {
    // Évitons de lancer le test si la connexion a échoué au SetUp
    ASSERT_TRUE(brokerDisponible) << "Test annulé : Broker non disponible.";

    std::atomic<bool> messageRecu{ false };
    std::string messageContenu = "";

    // A. Test de la souscription
    bool estAbonne = mqttTest->souscrire(TEST_TOPIC, [&](std::string topic, std::string payload) {
        messageContenu = payload;
        messageRecu = true; // Le drapeau se lève à la réception asynchrone
    });
    
    ASSERT_TRUE(estAbonne) << "L'abonnement au topic '" << TEST_TOPIC << "' a échoué.";

    // B. Test de la publication
    bool estPublie = mqttTest->publier(TEST_TOPIC, TEST_PAYLOAD);
    ASSERT_TRUE(estPublie) << "La publication du message a échoué.";

    // C. Attente asynchrone sécurisée de la réception (Timeout de 5 secondes)
    std::jthread horlogeTimeout([&messageRecu](std::stop_token stoken) {
        int secondesEcoulees = 0;
        while (!messageRecu && !stoken.stop_requested() && secondesEcoulees < 5) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            secondesEcoulees++;
        }
    });

    // On attend la fin du jthread (soit message reçu, soit timeout de 5s atteint)
    horlogeTimeout.join();

    // D. Vérifications finales via GoogleTest
    ASSERT_TRUE(messageRecu) << "Timeout ! Le message n'est jamais revenu au client (problème de Broker ?).";
    EXPECT_EQ(messageContenu, TEST_PAYLOAD) << "Le contenu du message reçu ne correspond pas à celui envoyé !";
}