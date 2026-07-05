#include "DevicesWidget.h"
#include "MqttManagerSimulator.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QDebug>

DevicesWidget::DevicesWidget(QWidget *parent) : QWidget(parent) {
    setupUI();

    // Connexions aux événements MQTT
    connect(&MqttManagerSimulator::instance(), &MqttManagerSimulator::deviceIdRecu, this, &DevicesWidget::onMqttIdRecu);
    connect(&MqttManagerSimulator::instance(), &MqttManagerSimulator::ordreSurveillanceRecu, this, &DevicesWidget::onMqttOrdreSurveillance);
}

void DevicesWidget::setupUI() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // ================= 1. ZONE DEVICE =================
    QGroupBox* boxDevice = new QGroupBox("Zone Device", this);
    QFormLayout* formLayout = new QFormLayout(boxDevice);

    m_leNom = new QLineEdit(this);
    
    m_cbType = new QComboBox(this);
    m_cbType->addItems({"GPU", "CPU", "RAM"});

    // Sliders avec affichage dynamique de la valeur
    m_slTemp = new QSlider(Qt::Horizontal, this);
    m_slTemp->setRange(20, 100);
    m_lblTempTarget = new QLabel("20°C", this);
    QHBoxLayout* layoutTemp = new QHBoxLayout();
    layoutTemp->addWidget(m_slTemp);
    layoutTemp->addWidget(m_lblTempTarget);
    connect(m_slTemp, &QSlider::valueChanged, [this](int val){ m_lblTempTarget->setText(QString::number(val) + "°C"); });

    m_slCharge = new QSlider(Qt::Horizontal, this);
    m_slCharge->setRange(0, 100);
    m_lblChargeTarget = new QLabel("0%", this);
    QHBoxLayout* layoutCharge = new QHBoxLayout();
    layoutCharge->addWidget(m_slCharge);
    layoutCharge->addWidget(m_lblChargeTarget);
    connect(m_slCharge, &QSlider::valueChanged, [this](int val){ m_lblChargeTarget->setText(QString::number(val) + "%"); });

    formLayout->addRow("Nom :", m_leNom);
    formLayout->addRow("Type :", m_cbType);
    formLayout->addRow("Température :", layoutTemp);
    formLayout->addRow("Taux de charge :", layoutCharge);

    // ================= 2. ZONE ACTIONS =================
    QGroupBox* boxActions = new QGroupBox("Zone Actions", this);
    QVBoxLayout* layoutActions = new QVBoxLayout(boxActions);

    QPushButton* btnAjouter = new QPushButton("Ajout Device", this);
    QPushButton* btnSupprimer = new QPushButton("Supprimer Device", this);
    QPushButton* btnSurveiller = new QPushButton("Surveiller Device", this);
    QPushButton* btnModifTemp = new QPushButton("Modifier Température", this);
    QPushButton* btnModifCharge = new QPushButton("Modifier Charge", this);

    layoutActions->addWidget(btnAjouter);
    layoutActions->addWidget(btnSupprimer);
    layoutActions->addWidget(btnSurveiller);
    layoutActions->addWidget(btnModifTemp);
    layoutActions->addWidget(btnModifCharge);
    layoutActions->addStretch();

    // Connexions des boutons
    connect(btnAjouter, &QPushButton::clicked, this, &DevicesWidget::onAjouterClicked);
    connect(btnSupprimer, &QPushButton::clicked, this, &DevicesWidget::onSupprimerClicked);
    connect(btnSurveiller, &QPushButton::clicked, this, &DevicesWidget::onSurveillerClicked);
    connect(btnModifTemp, &QPushButton::clicked, this, &DevicesWidget::onModifierTempClicked);
    connect(btnModifCharge, &QPushButton::clicked, this, &DevicesWidget::onModifierChargeClicked);

    // ================= 3. ZONE LISTE =================
    QGroupBox* boxListe = new QGroupBox("Liste des Devices", this);
    QVBoxLayout* layoutListe = new QVBoxLayout(boxListe);
    m_listDevices = new QListWidget(this);
    layoutListe->addWidget(m_listDevices);

    // Assemblage final de l'écran
    mainLayout->addWidget(boxDevice, 2);
    mainLayout->addWidget(boxActions, 1);
    mainLayout->addWidget(boxListe, 2);
}

// Helper pour récupérer le Device actuellement sélectionné dans la liste visualisée
Device* DevicesWidget::getSelectedDevice() {
    QListWidgetItem* item = m_listDevices->currentItem();
    if (!item) return nullptr;
    
    QString nom = item->text();
    auto it = m_mapDevices.find(nom);
    if (it != m_mapDevices.end()) {
        return &(it->second);
    }
    return nullptr;
}

// --- SLOTS ACTIONS BUTTONS ---

void DevicesWidget::onAjouterClicked() {
    QString nom = m_leNom->text().trimmed();
    if (nom.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le nom ne peut pas être vide.");
        return;
    }
    if (m_mapDevices.find(nom) != m_mapDevices.end()) {
        QMessageBox::warning(this, "Erreur", "Ce device existe déjà.");
        return;
    }

    // 1. Ajout dans la Map locale
    Device d;
    d.id = -1; // Sera mis à jour par le retour MQTT
    d.nom = nom.toStdString();
    d.type = static_cast<DeviceType>(m_cbType->currentIndex());
    d.temperature = m_slTemp->value();
    d.charge = m_slCharge->value();

    m_mapDevices[nom] = d;

    // 2. Affichage graphique
    m_listDevices->addItem(nom);

    // 3. Envoi MQTT
    MsgAjoutDevice msg{ d.id, d.nom, d.type, d.temperature, d.charge};
    MqttManagerSimulator::instance().publierStructure("Ajouter Device", msg);

    // --- CODE SIMULATION ---
    // Simule une réponse instantanée du projet "Monitoring" pour le test (Ex: ID unique = 100 + taille)
    MqttManagerSimulator::instance().simulerReceptionIdDuMonitoring(nom, 100 + m_mapDevices.size());
}

void DevicesWidget::onSupprimerClicked() {
    Device* dev = getSelectedDevice();
    if (!dev) {
        QMessageBox::information(this, "Sélection", "Veuillez sélectionner un device dans la liste.");
        return;
    }

    if (dev->id == -1) {
        QMessageBox::warning(this, "Attente", "Ce device n'a pas encore reçu son ID unique.");
        return;
    }

    // Envoi MQTT
    MsgSuppression msg{dev->id};
    MqttManagerSimulator::instance().publierStructure("Supprimer Device", msg);

    // Nettoyage local
    QString nom = QString::fromStdString(dev->nom);;
    m_mapDevices.erase(nom);
    
    // Nettoyage IHM
    delete m_listDevices->currentItem();
}

void DevicesWidget::onSurveillerClicked() {
    Device* dev = getSelectedDevice();
    if (!dev || dev->id == -1) return;

    // Le sujet mentionne : publication avec le topic "Device"
    MsgDataInt msg{dev->id, 0}; // Payload non spécifié, on transmet l'ID
    MqttManagerSimulator::instance().publierStructure("Device", msg);

    // --- CODE SIMULATION ---
    // Simule la réception du message "Surveillé" envoyé par l'application de Monitoring
    MqttManagerSimulator::instance().simulerReceptionSurveillance(dev->id);
}

void DevicesWidget::onModifierTempClicked() {
    Device* dev = getSelectedDevice();
    if (!dev || dev->id == -1) return;

    // Mise à jour de la valeur actuelle du slider pour ce device
    dev->temperature = m_slTemp->value();

    MsgDataInt msg{dev->id, dev->temperature};
    MqttManagerSimulator::instance().publierStructure("Temperature", msg);
}

void DevicesWidget::onModifierChargeClicked() {
    Device* dev = getSelectedDevice();
    if (!dev || dev->id == -1) return;

    // Mise à jour de la valeur actuelle du slider pour ce device
    dev->charge = m_slCharge->value();

    MsgDataInt msg{dev->id, dev->charge};
    MqttManagerSimulator::instance().publierStructure("Charge", msg);
}

// --- SLOTS RETOURS MQTT ---

void DevicesWidget::onMqttIdRecu(const QString& nom, int id) {
    auto it = m_mapDevices.find(nom);
    if (it != m_mapDevices.end()) {
        it->second.id = id;
        qDebug() << "[IHM] ID" << id << "associé au Device" << nom;
    }
}

void DevicesWidget::onMqttOrdreSurveillance(int id) {
    // Recherche du device correspondant à l'ID reçu
    for (auto& [nom, dev] : m_mapDevices) {
        if (dev.id == id) {
            qDebug() << "[IHM] Le Device" << nom << "(ID:" << id << ") est maintenant SURVEILLÉ par le Monitoring.";
            // Ajoutez ici un changement visuel si désiré (ex: couleur de l'item)
            break;
        }
    }
}