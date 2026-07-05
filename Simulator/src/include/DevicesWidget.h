#ifndef DEVICESWIDGET_H
#define DEVICESWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <map>

#include <Structures.h>

class DevicesWidget : public QWidget {
    Q_OBJECT
public:
    explicit DevicesWidget(QWidget *parent = nullptr);

private slots:
    void onAjouterClicked();
    void onSupprimerClicked();
    void onSurveillerClicked();
    void onModifierTempClicked();
    void onModifierChargeClicked();
    
    // Slots connectés aux signaux du MqttManagerSimulator
    void onMqttIdRecu(const QString& nom, int id);
    void onMqttOrdreSurveillance(int id);

private:
    void setupUI();
    Device* getSelectedDevice();

    // Composants Interface Zone Device
    QLineEdit* m_leNom;
    QComboBox* m_cbType;
    QSlider* m_slTemp;
    QSlider* m_slCharge;
    QLabel* m_lblTempTarget;
    QLabel* m_lblChargeTarget;

    // Composants Zone Liste
    QListWidget* m_listDevices;

    // Stockage des données : Clef = Nom du Device
    std::map<QString, Device> m_mapDevices;
};

#endif // DEVICESWIDGET_H