//C'est ici que l'on fait le lien (le binding) entre ton C++ moderne type-safe et l'univers Qt.

#include "MainWindow.h"
#include <QMetaObject>
#include <QThread>

MainWindow::MainWindow(DevicesMonitoringEngine& engine, QWidget* parent)
    : QMainWindow(parent), m_engine(engine) 
{
    resize(400, 600);
    setWindowTitle("Tableau de bord de Monitoring");

    // Configuration de la zone défilante de l'UI
    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    
    auto* container = new QWidget(scrollArea);
    m_cardsLayout = new QVBoxLayout(container);
    m_cardsLayout->setAlignment(Qt::AlignTop);
    container->setLayout(m_cardsLayout);
    scrollArea->setWidget(container);
    setCentralWidget(scrollArea);

    // =========================================================================
    // LA MAGIE DU CONCEPT : ABONNEMENT TYPE-SAFE VIA TON OBSERVABLECOLLECTION
    // =========================================================================
    
    // Abonnement à l'ajout
    m_engine.getDevices().subscribe(this, &MainWindow::onDeviceAdded, Notifications::ItemAdded);

    // Abonnement à la suppression
    m_engine.getDevices().subscribe(this, &MainWindow::onDeviceRemoved, Notifications::ItemRemoved);

    // Abonnement à la mise a jour
    m_engine.getDevices().subscribe(this, &MainWindow::onDeviceUpdated, Notifications::ItemUpdated);
}

void MainWindow::onDeviceAdded(int id) {
    // SÉCURITÉ THREAD : Si le moteur appelle cette fonction depuis son thread secondaire,
    // invokeMethod va ré-aiguiller intelligemment l'appel sur le thread principal de Qt.
    if (thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, [this, id]() { onDeviceAdded(id); }, Qt::QueuedConnection);
        return;
    }

    // Le moteur vient d'ajouter l'élément, on peut chercher ses infos actuelles.
    // (Dans un vrai projet, on ferait une méthode getById dans la collection)
    // Pour l'exemple, on simule la création d'une carte générique avec l'ID reçu :
    Device fakeDevice{id, "Périphérique dynamique " + std::to_string(id), DeviceType::GPU, 30.0f, 15.0f};

    auto* card = new DeviceWidget(fakeDevice, this);
    m_cardsLayout->addWidget(card);
    m_activeWidgets[id] = card;

    // Si l'utilisateur clique sur "Simuler déconnexion" dans la carte, on prévient le moteur
    connect(card, &DeviceWidget::disconnectionRequested, [this](int devId) {
        m_engine.disconnection(devId);
    });
}

void MainWindow::onDeviceRemoved(int id) {
    // SÉCURITÉ THREAD
    if (thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, [this, id]() { onDeviceRemoved(id); }, Qt::QueuedConnection);
        return;
    }

    // On cherche si le widget existe dans notre map UI
    auto it = m_activeWidgets.find(id);
    if (it != m_activeWidgets.end()) {
        DeviceWidget* card = it->second;
        
        m_cardsLayout->removeWidget(card); // On l'enlève du layout
        card->deleteLater();               // On demande à Qt de le détruire proprement
        
        m_activeWidgets.erase(it);         // On l'enlève de notre suivi
    }
}


// 1. Le moteur notifie que les données d'un appareil ont changé (ex: Notification::ItemUpdated)
// 2. La MainWindow reçoit l'ID de l'appareil modifié et retrouve le widget associé :
void MainWindow::onDeviceUpdated(int id) {
    if (thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, [this, id]() {
            onDeviceUpdated(id);
            }, Qt::QueuedConnection);
        return;
    }

    auto it = m_activeWidgets.find(id);
    if (it != m_activeWidgets.end()) {
        const Device* device = m_engine.getDevices().getById(id);
        if (device) {
            it->second->updateData(device->temperature, device->charge);
        }
    }
}
