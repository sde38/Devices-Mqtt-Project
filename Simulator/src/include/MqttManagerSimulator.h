#ifndef MQTTMANAGERSIMULATOR_H
#define MQTTMANAGERSIMULATOR_H

#include <QObject>
#include "Structures.h"
#include <MqttManager.h>

class MqttManagerSimulator : public QObject {
    Q_OBJECT
public:
    static MqttManagerSimulator& instance() {
        static MqttManagerSimulator ins;
        return ins;
    }

    // Fonctions de publication surchargées pour chaque structure
    template <typename T>
    void publierStructure(const QString& topic, const T& donnee) {
        std::string payload = MqttSerializer<T>::toString(donnee);
        qDebug() << "[MQTT-SIM] Publié sur" << topic
            << ":" << QString::fromStdString(payload);
    }

    // Simulation d'une réception de message depuis le projet Monitoring
    void simulerReceptionIdDuMonitoring(const QString& nomDevice, int idGenere) {
        emit deviceIdRecu(nomDevice, idGenere);
    }

    void simulerReceptionSurveillance(int idDevice) {
        emit ordreSurveillanceRecu(idDevice);
    }

signals:
    void deviceIdRecu(const QString& nom, int id);
    void ordreSurveillanceRecu(int id);

private:
    MqttManagerSimulator() = default;
};

#endif // MQTTMANAGERSIMULATOR_H