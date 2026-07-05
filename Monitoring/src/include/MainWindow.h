// C'est la fenêtre principale de l'application. Elle contient le layout où s'empilent les cartes 
// et elle s'abonne aux événements de ta collection C++20.

#pragma once
#include <QMainWindow>
#include <QVBoxLayout>
#include <QScrollArea>
#include <map>

#include "DevicesMonitoringEngine.h"
#include "DeviceWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    DevicesMonitoringEngine& m_engine;
    QVBoxLayout* m_cardsLayout; // Layout où on ajoute/supprime les widgets
    
    // Permet de retrouver rapidement le Widget associé à un ID de périphérique
    std::map<int, DeviceWidget*> m_activeWidgets;

public:
    explicit MainWindow(DevicesMonitoringEngine& engine, QWidget* parent = nullptr);
    ~MainWindow() = default;

private:
    // Les slots/méthodes qui seront appelés par les callbacks de ta collection
    void onDeviceAdded(int id);
    void onDeviceRemoved(int id);
    void onDeviceUpdated(int id);
};