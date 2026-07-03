#include "MqttManagerMonitoring.h"

void MqttManagerMonitoring::demarrerAbonnements() {
    souscrireEtRelayer<MsgAjoutDevice>(TOPIC_AJOUT, [this](const MsgAjoutDevice& donnee) {
        emit msgAjoutRecu(donnee);
    });

    souscrireEtRelayer<MsgSuppression>(TOPIC_SUPPRESSION, [this](const MsgSuppression& donnee) {
        emit msgSuppressionRecu(donnee);
    });

    souscrireEtRelayer<MsgDataInt>(TOPIC_SURVEILLANCE, [this](const MsgDataInt& donnee) {
        // MsgDataInt.id sert ici a transporter l'ID de l'appareil a surveiller.
        emit ordreSurveillanceRecu(donnee.id);
    });
}

bool MqttManagerMonitoring::publierDonnee(const MsgDataInt& donnee) {
    return m_mqtt->publierStructure(TOPIC_TEMPERATURE, donnee);
}
