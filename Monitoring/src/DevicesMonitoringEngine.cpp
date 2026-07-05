// Point d'attention pour l'etape Qt (Le Multi-threading)
// La methode m_devices.add() est appelee soit depuis le thread secondaire
// (m_simulationThread), soit depuis un handler declenche par un signal Qt
// (deja livre sur le thread principal grace a MqttManagerMonitoring).
//
// Regle d'or de Qt : on ne doit jamais modifier l'interface graphique depuis
// un autre thread que le thread principal. Pour la couche UI (MainWindow),
// utiliser QMetaObject::invokeMethod dans le callback d'abonnement a
// ObservableCollection permet de renvoyer l'ordre d'affichage sur le thread
// principal en toute securite.

#include "DevicesMonitoringEngine.h"
#include "MqttManagerMonitoring.h"
#include <chrono>

DevicesMonitoringEngine::DevicesMonitoringEngine(MqttManagerMonitoring& mqtt)
    : m_mqtt(mqtt)
{
    QObject::connect(&m_mqtt, &MqttManagerMonitoring::msgAjoutRecu,
                      &m_connexionContext,
                      [this](MsgAjoutDevice donnee) { onMsgAjoutRecu(donnee); });

    QObject::connect(&m_mqtt, &MqttManagerMonitoring::msgSuppressionRecu,
                      &m_connexionContext,
                      [this](MsgSuppression donnee) { onMsgSuppressionRecu(donnee); });

    QObject::connect(&m_mqtt, &MqttManagerMonitoring::ordreSurveillanceRecu,
                      &m_connexionContext,
                      [this](int id) { onOrdreSurveillanceRecu(id); });
}

DevicesMonitoringEngine::~DevicesMonitoringEngine() {
    stop();
}

void DevicesMonitoringEngine::start() {
    if (m_running) { return; }

    m_running = true;

    // Lancement du thread C++20 (m_simulationThread s'occupe de sa propre duree de vie)
    m_simulationThread = std::jthread(&DevicesMonitoringEngine::run, this);
}

void DevicesMonitoringEngine::stop() {
    m_running = false;
    if (m_simulationThread.joinable()) {
        m_simulationThread.join();
    }
}

void DevicesMonitoringEngine::disconnection(int id) {
    // Utilise la methode removeById que l'on a planifiee pour ObservableCollection
    m_devices.removeById(id);
}

void DevicesMonitoringEngine::run() {

    while (m_running) {
        // On attend 3 secondes entre chaque evenement de simulation
        std::this_thread::sleep_for(std::chrono::seconds(3));
        if (!m_running) { break; }


    }
}

// ============================================================================
// Handlers MQTT (invoques sur le thread Qt principal, voir constructeur)
// ============================================================================

void DevicesMonitoringEngine::onMsgAjoutRecu(const MsgAjoutDevice& donnee) {
    // NOTE : mapping MsgAjout -> Device. A adapter selon la regle d'attribution
    // d'ID reelle (ici on reutilise l'id transmis par le Simulator tel quel ;
    // si Monitoring doit generer son propre ID, c'est ici qu'il faut le faire
    // avant de construire idAttribue ci-dessous).
    Device d;
    d.id          = donnee.id;
    d.nom         = donnee.nom;
    d.type        = donnee.type;
    d.temperature = static_cast<float>(donnee.temperature);
    d.charge      = static_cast<float>(donnee.charge);

    m_devices.add(d);

    MsgDataInt idData;
    idData.data = 1;
    idData.id  = d.id;
    m_mqtt.publierDonnee(idData);
}

void DevicesMonitoringEngine::onMsgSuppressionRecu(const MsgSuppression& donnee) {
    m_devices.removeById(donnee.id);
}

void DevicesMonitoringEngine::onOrdreSurveillanceRecu(int id) {
    // NOTE : point d'extension. A ce stade, on sait qu'un ordre de surveillance
    // a ete recu pour l'appareil `id`. La logique de suivi (marquer l'appareil
    // comme "surveille", declencher l'envoi periodique de MsgDataInt via
    // m_mqtt.publierDonnee(...), etc.) reste a definir selon le besoin metier.
    (void)id;
}
