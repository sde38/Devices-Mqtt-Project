// Ce composant représente la carte visuelle d'un seul périphérique (un composant graphique individuel).

#pragma once
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include "HardwareDevice.h"

class HardwareWidget : public QWidget {
    Q_OBJECT
private:
    int m_deviceId;
    QLabel* m_nameLabel;
    QProgressBar* m_loadBar;
    QLabel* m_tempLabel;

public:
    explicit HardwareWidget(const HardwareDevice& device, QWidget* parent = nullptr);
    [[nodiscard]]  int getDeviceId() const { return m_deviceId; }
    
    // Permet de mettre à jour les données de la carte plus tard si besoin
    void updateData(float temp, float load);

signals:
    // Signal émis si l'utilisateur clique sur le bouton "Déconnecter" de la carte
    void disconnectionRequested(int id);
};