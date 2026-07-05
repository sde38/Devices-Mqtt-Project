#include <QCoreApplication>
#include <QDebug>

#include "MqttManagerMonitoring.h"
#include "DevicesMonitoringEngine.h"

// ============================================================================
// Chaine de construction, sans singleton :
//
//   1) QCoreApplication : necessaire pour que la boucle d'evenements Qt
//      tourne (les QMetaObject::invokeMethod(..., Qt::QueuedConnection)
//      utilises par MqttManagerMonitoring ne se declenchent qu'une fois
//      app.exec() lance).
//   2) MqttManagerMonitoring : construit directement avec l'adresse du
//      broker et l'identifiant client (plus de configurer() separe).
//   3) DevicesMonitoringEngine : recoit MqttManagerMonitoring& en parametre
//      de constructeur (injection de dependance explicite).
//
// Adapter serverAddress / clientId a l'environnement reel (fichier de
// configuration, variables d'environnement, arguments de ligne de commande...).
// ============================================================================
int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    MqttManagerMonitoring mqttMonitoring("tcp://localhost:1883", "monitoring-client");

    if (!mqttMonitoring.connecter()) {
        qWarning() << "[main] Impossible de se connecter au broker MQTT";
        return 1;
    }

    mqttMonitoring.demarrerAbonnements();

    DevicesMonitoringEngine engine(mqttMonitoring);
    engine.start();

    int result = app.exec();

    engine.stop();
    mqttMonitoring.deconnecter();

    return result;
}
