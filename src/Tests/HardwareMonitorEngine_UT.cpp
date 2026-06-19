// On va tester le comportement de la ObservableCollection à l'intérieur du moteur, et vérifier que les callbacks de notification
// s'exécutent correctement lorsqu'on ajoute ou retire un élément.

#include <gtest/gtest.h>
#include "HardwareMonitoringEngine.h"

// Classe "Mock" minimale simulant un QObject pour satisfaire le concept QObjectDerived
class MockSubscriber : public QObject {
public:
    int lastNotifiedId = -1;
    void onItemLastNotified(int id) { lastNotifiedId = id; }
};

TEST(HardwareMonitoringEngineTest, SubscriptionAndNotificationChain) {
    HardwareMonitoringEngine engine;
    MockSubscriber subscriber;

    // On s'abonne à l'événement d'ajout
    engine.getDevices().subscribe(&subscriber, &MockSubscriber::onItemLastNotified, Notifications::ItemAdded);

    // On crée un périphérique et on l'ajoute directement à la collection du moteur
    HardwareDevice device{99, "Test Device", "USB", 20.0f, 0.0f};
    engine.getDevices().add(device);

    // On vérifie que notre abonné a bien reçu l'ID 99 via le callback du concept C++20
    EXPECT_EQ(subscriber.lastNotifiedId, 99);
}

TEST(HardwareMonitoringEngineTest, ManualDisconnectionExclusion) {
    HardwareMonitoringEngine engine;
    MockSubscriber subscriber;

    engine.getDevices().subscribe(&subscriber, &MockSubscriber::onItemLastNotified, Notifications::ItemRemoved);

    // On déclenche la suppression de l'ID par défaut (le moteur initialise l'ID 1 dans son constructeur)
    engine.simulateDisconnection(1);

    // L'abonné configuré sur "ItemRemoved" doit avoir reçu l'ID 1
    EXPECT_EQ(subscriber.lastNotifiedId, 1);
}