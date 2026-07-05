// Ce fichier declare la classe du moteur. Il integre un mecanisme pour mettre a jour les donnees
// en arriere-plan sans bloquer l'application.
// On utilise un std::jthread (C++20) pour simuler l'arrivee et le depart de peripheriques.

// Pour la couche metier, le DevicesMonitoringEngine est le chef d'orchestre de ton application.
// C'est lui qui possede et gere la ObservableCollection, et qui simule (ou recupere) l'activite
// du materiel informatique.

// MODIFICATION : le moteur recoit desormais une reference vers MqttManagerMonitoring (injectee
// par l'appelant, typiquement main()) et s'y connecte via un QObject de contexte prive
// (m_connexionContext). Cela evite de faire heriter DevicesMonitoringEngine de QObject tout en
// beneficiant de connect() (thread-safety + duree de vie automatique des connexions), et evite
// aussi de dependre d'un singleton MqttManagerMonitoring::instance().

#pragma once
#include <thread>
#include <atomic>
#include <QObject>
#include "ObservableCollection.h"

#include <Structures.h>

class MqttManagerMonitoring; // forward declaration : pas besoin du header complet ici

class DevicesMonitoringEngine {
private:
    // La collection reactive qui contient nos peripheriques
    ObservableCollection<Device> m_devices;

    // Gestion du thread de simulation arriere-plan
    std::jthread m_simulationThread;
    std::atomic<bool> m_running{false};

    // Reference vers l'adaptateur MQTT, pour pouvoir publier depuis les handlers
    MqttManagerMonitoring& m_mqtt;

    // Objet QObject "nu" utilise uniquement comme contexte de connexion pour
    // QObject::connect(). Sa destruction (avec celle du moteur) coupe
    // automatiquement les connexions : pas de risque d'appel sur un this detruit.
    QObject m_connexionContext;

    // Fonction interne executee par le thread
    void run();

    // Handlers appeles (sur le thread Qt principal) quand un signal MQTT arrive
    void onMsgAjoutRecu(const MsgAjoutDevice& donnee);
    void onMsgSuppressionRecu(const MsgSuppression& donnee);
    void onOrdreSurveillanceRecu(int id);

public:
    explicit DevicesMonitoringEngine(MqttManagerMonitoring& mqtt);
    ~DevicesMonitoringEngine();

    // Permet de demarrer et arreter la surveillance/simulation
    void start();
    void stop();

    // Donne un acces en lecture seule a la collection pour que l'UI puisse s'y abonner
    ObservableCollection<Device>& getDevices() { return m_devices; }

    // Fonction utilitaire pour forcer manuellement la suppression d'un composant (ex: via l'UI)
    void disconnection(int id);
};
