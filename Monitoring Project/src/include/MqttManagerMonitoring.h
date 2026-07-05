#ifndef MQTTMANAGERMONITORING_H
#define MQTTMANAGERMONITORING_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QMetaObject>
#include <memory>

#include "Structures.h"
#include <MqttManager.h>

// ============================================================================
// Pendant "reel" de MqttManagerSimulator, cote projet Monitoring.
//
// Encapsule un MqttManager (facade Pimpl de la librairie statique mqttCom)
// et re-expose ses fonctionnalites sous forme de signaux Qt, afin que le
// reste de l'application (DevicesMonitoringEngine, MainWindow, ...) n'ait
// jamais a connaitre Paho ni le thread interne du client MQTT.
//
// REGLE D'OR THREADING :
// Les callbacks fournis a MqttManager::souscrireStructure() sont invoques
// depuis le thread interne du client MQTT (thread Paho), jamais depuis le
// thread Qt principal. On protege donc systematiquement l'emission des
// signaux via QMetaObject::invokeMethod(..., Qt::QueuedConnection), exactement
// comme le fait deja MainWindow pour les notifications d'ObservableCollection.
//
// MODIFICATION : plus de singleton. L'instance est creee explicitement par
// l'appelant (typiquement main()) et injectee la ou elle est necessaire
// (ex: dans le constructeur de DevicesMonitoringEngine). m_mqtt est
// construit directement dans le constructeur : il n'y a donc plus d'etat
// intermediaire "configure mais pas connecte", et plus besoin de verifier
// sa nullite dans chaque methode.
// ============================================================================
class MqttManagerMonitoring : public QObject {
    Q_OBJECT

public:
    MqttManagerMonitoring(const std::string& serverAddress,
                           const std::string& clientId,
                           QObject* parent = nullptr)
        : QObject(parent)
        , m_mqtt(std::make_unique<MqttManager>(serverAddress, clientId))
    {}

    MqttManagerMonitoring(const MqttManagerMonitoring&) = delete;
    MqttManagerMonitoring& operator=(const MqttManagerMonitoring&) = delete;

    bool connecter() {
        return m_mqtt->connecter();
    }

    void deconnecter() {
        m_mqtt->deconnecter();
    }

    // Abonne le client aux topics utiles au projet Monitoring et relie chaque
    // message recu a l'emission (thread-safe) du signal Qt correspondant.
    // A appeler une fois, apres connecter().
    void demarrerAbonnements();

    // --- Publications sortantes (Monitoring -> Simulator / broker) ---

    // Publie une mise a jour de telemetrie (temperature/charge) pour un
    // appareil surveille.
    bool publierDonnee(const MsgDataInt& donnee);

signals:
    // Le projet Simulator annonce un nouvel appareil : a Monitoring de lui
    // attribuer un ID (cf. DevicesMonitoringEngine::onMsgAjoutRecu)
    // puis d'appeler publierIdAttribue().
    void msgAjoutRecu(MsgAjoutDevice donnee);

    // Le projet Simulator (ou le broker) signale une deconnexion.
    void msgSuppressionRecu(MsgSuppression donnee);

    // Ordre de surveillance recu pour un ID donne.
    void ordreSurveillanceRecu(int id);

private:
    // Petite fabrique commune : enveloppe un callback "metier" (qui s'attend
    // a etre execute sur le thread Qt) avec le renvoi thread-safe.
    template <typename T, typename Fonction>
    void souscrireEtRelayer(const std::string& topic, Fonction relais) {
        m_mqtt->souscrireStructure<T>(topic,
            [this, relais](std::string /*topicRecu*/, T donnee) {
                QMetaObject::invokeMethod(this, [relais, donnee]() {
                    relais(donnee);
                }, Qt::QueuedConnection);
            });
    }

    // Toujours construit (voir constructeur) : plus de verification de
    // nullite necessaire dans les methodes ci-dessus.
    std::unique_ptr<MqttManager> m_mqtt;
};

#endif // MQTTMANAGERMONITORING_H
