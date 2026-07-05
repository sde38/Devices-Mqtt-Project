#include <gtest/gtest.h>
#include "MqttManager.h"
#include <chrono>
#include <thread>
#include <atomic>
#include <string>

// Configuration globale pour les tests
const std::string BROKER_ADDRESS = "tcp://localhost:1883";
const std::string TEST_TOPIC = "test/mqttcomm/unittest";
const std::string TEST_PAYLOAD = "Hello_MQTT_Unitaire";

// --- FIXTURE DE TEST ---
// Permet de partager la configuration et l'initialisation entre les tests
class MqttManagerTest : public ::testing::Test {
protected:
	// Cette fonction s'exécute AVANT chaque test
	void SetUp() override {
		mqttManager = std::make_unique<MqttManager>(BROKER_ADDRESS, "Client_UT_Docker");

		// Tentative de connexion initiale
		brokerDisponible = mqttManager->connecter();
		if (!brokerDisponible) {
			std::cerr << "[WARNING] Le broker MQTT n'est pas joignable sur "
				<< BROKER_ADDRESS << ". Certains tests vont être ignorés.\n";
		}
	}

	// Cette fonction s'exécute APRÈS chaque test
	void TearDown() override {
		if (mqttManager && brokerDisponible) {
			mqttManager->deconnecter();
		}
		mqttManager.reset();
	}

	std::unique_ptr<MqttManager> mqttManager;
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
	bool estAbonne = mqttManager->souscrire(TEST_TOPIC, [&](std::string topic, std::string payload) {
		messageContenu = payload;
		messageRecu = true; // Le drapeau se lève à la réception asynchrone
		});

	ASSERT_TRUE(estAbonne) << "L'abonnement au topic '" << TEST_TOPIC << "' a échoué.";

	// B. Test de la publication
	bool estPublie = mqttManager->publier(TEST_TOPIC, TEST_PAYLOAD);
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

// --- NOUVEAUX TESTS COMPLÉMENTAIRES ---

// 3. Test des comportements en mode Déconnecté
// Ce test vérifie que les méthodes publiques échouent proprement et ne plantent pas si le client n'est pas connecté.
TEST_F(MqttManagerTest, TestComportementHorsConnexion) {
	// On crée un manager local exprès pour maîtriser son état (non connecté)
	MqttManager managerDeconnecte(BROKER_ADDRESS, "Client_UT_Offline");

	// Le manager n'a pas appelé .connecter(), il est donc hors ligne.

	// A. Tenter de publier hors connexion doit retourner false
	bool publicationReussie = managerDeconnecte.publier(TEST_TOPIC, "Payload");
	EXPECT_FALSE(publicationReussie) << "La publication aurait dû échouer car le client n'est pas connecté.";

	// B. Tenter de s'abonner hors connexion doit retourner false
	bool abonnementReussi = managerDeconnecte.souscrire(TEST_TOPIC, [](std::string, std::string) {});
	EXPECT_FALSE(abonnementReussi) << "L'abonnement aurait dû échouer car le client n'est pas connecté.";
}

// 4. Test de la deconnexion explicite
// Ce test valide que la méthode publique deconnecter() coupe bien la liaison et empêche les actions futures.
TEST_F(MqttManagerTest, TestDeconnexionExplicite) {
	ASSERT_TRUE(brokerDisponible) << "Test annulé : Broker non disponible.";

	// Le client est connecté grâce au SetUp. On le déconnecte explicitement.
	mqttManager->deconnecter();

	// On vérifie que les appels suivants échouent bien
	bool estPublie = mqttManager->publier(TEST_TOPIC, TEST_PAYLOAD);
	EXPECT_FALSE(estPublie) << "La publication après une déconnexion explicite doit échouer.";
}

// 5. Test de la gestion de plusieurs Topics simultanés (Multi-callbacks)
// Ce test vérifie que la std::map interne (m_callbacks) route correctement les messages vers les bonnes lambdas.
TEST_F(MqttManagerTest, TestSouscriptionsMultiples) {
	ASSERT_TRUE(brokerDisponible) << "Test annulé : Broker non disponible.";

	const std::string TOPIC_A = "test/multi/topic_A";
	const std::string TOPIC_B = "test/multi/topic_B";

	std::atomic<bool> recuA{ false };
	std::atomic<bool> recuB{ false };

	// Abonnement au Topic A
	bool abonnementA = mqttManager->souscrire(TOPIC_A, [&](std::string, std::string) {
		recuA = true;
		});
	// Abonnement au Topic B
	bool abonnementB = mqttManager->souscrire(TOPIC_B, [&](std::string, std::string) {
		recuB = true;
		});

	ASSERT_TRUE(abonnementA && abonnementB) << "L'un des abonnements multiples a échoué.";

	// On publie UNIQUEMENT sur le Topic B
	bool estPublieB = mqttManager->publier(TOPIC_B, "Payload B");
	ASSERT_TRUE(estPublieB);

	// Attente asynchrone sécurisée de 2 secondes max
	std::jthread horloge([&recuB](std::stop_token stoken) {
		int ms = 0;
		while (!recuB && !stoken.stop_requested() && ms < 2000) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			ms += 100;
		}
		});
	horloge.join();

	// Vérifications : Le message sur B a dû être reçu, mais A ne doit pas avoir bougé !
	EXPECT_TRUE(recuB) << "Le message sur le Topic B n'a pas été capté.";
	EXPECT_FALSE(recuA) << "Erreur de routage : Le Topic A a reçu un signal destiné au Topic B.";
}

// 6. Test aux limites : Publication d'un message vide
// Ce test vérifie que le système supporte les payloads vides ("") sans crasher.
TEST_F(MqttManagerTest, TestPayloadVide) {
	ASSERT_TRUE(brokerDisponible) << "Test annulé : Broker non disponible.";

	std::atomic<bool> messageRecu{ false };
	std::string messageContenu = "Initialement non vide";

	// Abonnement
	mqttManager->souscrire(TEST_TOPIC, [&](std::string, std::string payload) {
		messageContenu = payload;
		messageRecu = true;
		});

	// Publication d'une chaîne vide
	mqttManager->publier(TEST_TOPIC, "");

	// Attente
	std::jthread horloge([&messageRecu](std::stop_token stoken) {
		int secondes = 0;
		while (!messageRecu && !stoken.stop_requested() && secondes < 3) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			secondes++;
		}
		});
	horloge.join();

	ASSERT_TRUE(messageRecu) << "Le message vide n'est jamais revenu.";
	EXPECT_EQ(messageContenu, "") << "Le contenu reçu aurait dû être une chaîne vide.";
}

// 7. Test de charge (Stress Test) : Réception d'un flux important de messages
TEST_F(MqttManagerTest, TestFluxImportantMessages) {
	ASSERT_TRUE(brokerDisponible) << "Test annulé : Broker non disponible.";

	const int NOMBRE_MESSAGES = 1000; // Volume du flux

	// std::atomic garantit que chaque incrémentation messagesRecusCompteur++ se fait de manière sécurisée, 
	// sans qu'un thread n'écrase le calcul d'un autre (race condition).
	std::atomic<int> messagesRecusCompteur{ 0 };

	// A. Souscription avec une lambda légère qui incrémente le compteur atomique
	bool estAbonne = mqttManager->souscrire(TEST_TOPIC, [&](std::string, std::string) {
		// Note : elle ne fait qu'une incrémentation en mémoire. Si vous commenciez à faire des std::cout ou des écritures 
		// dans un fichier à chaque message reçu, le thread MQTT saturerait et vous perdriez des messages.
		messagesRecusCompteur++; // Incrémentation thread-safe
		});
	ASSERT_TRUE(estAbonne);

	// B. Publication massive et rapide (Le flux important)
	for (int i = 0; i < NOMBRE_MESSAGES; ++i) {
		std::string payload = "Msg_" + std::to_string(i);
		bool publie = mqttManager->publier(TEST_TOPIC, payload);

		// Si une seule publication échoue (ex: buffer plein), on arrête le test
		ASSERT_TRUE(publie) << "La publication a échoué au message numéro : " << i;
	}

	// C. Attente asynchrone adaptative
	// On laisse un peu plus de temps (10 secondes max) car traiter 1000 messages prend du temps
	std::jthread horloge([&messagesRecusCompteur, NOMBRE_MESSAGES](std::stop_token stoken) {
		int millisecondesEcoulees = 0;
		while (messagesRecusCompteur < NOMBRE_MESSAGES && !stoken.stop_requested() && millisecondesEcoulees < 10000) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			millisecondesEcoulees += 100;
		}
		});
	horloge.join();

	// D. Vérification finale
	EXPECT_EQ(messagesRecusCompteur.load(), NOMBRE_MESSAGES)
		<< "Perte de messages détectée ! Seulement " << messagesRecusCompteur
		<< " sur " << NOMBRE_MESSAGES << " ont été reçus.";
}

// 8. Test aux limites : Écrasement massif de callbacks sur le même topic
// Vérifier que s'abonner et se désabonner (ou écraser des abonnements) en boucle 
// ne provoque pas de fuite de mémoire (memory leak) ou de corruption dans votre std::map.
TEST_F(MqttManagerTest, TestRobustesseEcrasementCallbacks) {
	ASSERT_TRUE(brokerDisponible) << "Test annulé : Broker non disponible.";

	// On s'abonne 500 fois d'affilée au MÊME topic en écrasant la lambda à chaque fois
	for (int i = 0; i < 500; ++i) {
		bool estAbonne = mqttManager->souscrire(TEST_TOPIC, [i](std::string, std::string) {
			// Un callback temporaire qui capture 'i'
			});
		ASSERT_TRUE(estAbonne);
	}

	// À la fin, la map ne doit contenir qu'UN SEUL élément (le dernier)
	// et la mémoire des 499 précédentes lambdas doit avoir été proprement libérée par std::move
	bool estPublie = mqttManager->publier(TEST_TOPIC, "Test");
	EXPECT_TRUE(estPublie);
}

// 9. Test aux limites : UTF-8 complexe et caractères de contrôle (\0)
// MQTT est conçu pour transporter n'importe quel type de données (du texte JSON, mais aussi des images 
// ou des structures binaires). Ce test vérifie que votre code supporte les caractères invisibles ou coupés 
// (comme le caractère de fin de chaîne \0 au milieu d'un message).
TEST_F(MqttManagerTest, TestPayloadCaracteresSpeciaux) {
	ASSERT_TRUE(brokerDisponible) << "Test annulé : Broker non disponible.";

	// Un payload contenant des Emojis, du chinois, et un caractère nul '\0' au milieu
	std::string payloadSpecial = "Start__🚀_中文_\0_End";

	std::atomic<bool> messageRecu{ false };
	std::string messageContenu = "";

	mqttManager->souscrire(TEST_TOPIC, [&](std::string, std::string payload) {
		messageContenu = payload;
		messageRecu = true;
		});

	mqttManager->publier(TEST_TOPIC, payloadSpecial);

	// Attente asynchrone
	std::jthread horloge([&messageRecu](std::stop_token stoken) {
		int ms = 0;
		while (!messageRecu && !stoken.stop_requested() && ms < 3000) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			ms += 100;
		}
		});
	horloge.join();

	ASSERT_TRUE(messageRecu);
	// On vérifie que la chaîne n'a pas été coupée au premier '\0' rencontrée
	EXPECT_EQ(messageContenu.size(), payloadSpecial.size());
	EXPECT_EQ(messageContenu, payloadSpecial);
}


// 10. Test aux limites : Topic très long et arborescence profonde
//	La spécification MQTT autorise des topics très longs(jusqu'à 65 535 caractères) et profonds 
//  (avec beaucoup de /). Ce test valide que la clé de votre std::map et le buffer réseau gèrent 
//  les longues chaînes.
TEST_F(MqttManagerTest, TestTopicTresLong) {
	ASSERT_TRUE(brokerDisponible) << "Test annulé : Broker non disponible.";

	// Génération d'un topic du style : niveau1/niveau2/niveau3/... sur 1000 caractères
	std::string topicTresLong = "test";
	for (int i = 0; i < 150; ++i) {
		topicTresLong += "/niveau" + std::to_string(i);
	}

	std::atomic<bool> messageRecu{ false };

	bool estAbonne = mqttManager->souscrire(topicTresLong, [&](std::string, std::string) {
		messageRecu = true;
		});
	ASSERT_TRUE(estAbonne) << "Le broker ou le client a refusé ce format de topic.";

	mqttManager->publier(topicTresLong, "Donnée");

	std::jthread horloge([&messageRecu](std::stop_token stoken) {
		int ms = 0;
		while (!messageRecu && !stoken.stop_requested() && ms < 3000) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			ms += 100;
		}
		});
	horloge.join();

	EXPECT_TRUE(messageRecu);
}