#include "DeviceWidget.h"

DeviceWidget::DeviceWidget(const Device& device, QWidget* parent)
    : QWidget(parent), m_deviceId(device.id) 
{
    // Design basique de la carte (Style CSS optionnel pour faire joli)
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("DeviceWidget { background-color: #2d2d2d; color: white; border-radius: 8px; padding: 10px; }");

    auto* layout = new QVBoxLayout(this);
    
    m_nameLabel = new QLabel(QString::fromStdString(device.nom), this);
    m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    
    m_loadBar = new QProgressBar(this);
    m_loadBar->setRange(0, 100);
    m_loadBar->setValue(static_cast<int>(device.charge));
    
    m_tempLabel = new QLabel(QString("Température: %1 °C").arg(device.temperature), this);

    auto* closeButton = new QPushButton("Simuler Déconnexion", this);
    closeButton->setStyleSheet("background-color: #c0392b; color: white; border: none; padding: 5px; border-radius: 4px;");

    layout->addWidget(m_nameLabel);
    layout->addWidget(new QLabel("Utilisation :", this));
    layout->addWidget(m_loadBar);
    layout->addWidget(m_tempLabel);
    layout->addWidget(closeButton);

    // Connexion du bouton au signal pour remonter l'info à la MainWindow
    connect(closeButton, &QPushButton::clicked, [this]() {
        emit disconnectionRequested(m_deviceId);
    });
}

void DeviceWidget::updateData(float temp, float load) {
    m_loadBar->setValue(static_cast<int>(load));
    m_tempLabel->setText(QString("Température: %1 °C").arg(temp));
}