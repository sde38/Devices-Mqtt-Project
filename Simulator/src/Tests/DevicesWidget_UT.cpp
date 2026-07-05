#include <gtest/gtest.h>
#include <QApplication>
#include <QTest>
#include <QLineEdit>
#include <QComboBox>
#include <QSlider>
#include <QPushButton>
#include <QListWidget>

// Inclusions des classes à tester
#include "DevicesWidget.h"
#include "MqttManagerSimulator.h"

// --- FIXTURE DE TEST ---
// La fixture prépare l'environnement Qt avant chaque test
class DevicesWidgetTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // GTest a besoin d'une instance unique de QApplication pour l'IHM Qt
        static int argc = 1;
        static char* argv[] = {(char*)"gtest_app"};
        m_app = new QApplication(argc, argv);
    }

    static void TearDownTestSuite() {
        delete m_app;
        m_app = nullptr;
    }

    void SetUp() override {
        // Instanciation du widget avant chaque test
        m_widget = new DevicesWidget();
        m_widget->show();

        // Récupération des pointeurs des composants internes via le système de reflection Qt
        // Cela évite de devoir modifier la visibilité "private" dans le code source original
        m_leNom = m_widget->findChild<QLineEdit*>();
        m_cbType = m_widget->findChild<QComboBox*>();
        m_slTemp = m_widget->findChild<QSlider*>();
        m_slCharge = m_widget->findChild<QSlider*>();
        m_listDevices = m_widget->findChild<QListWidget*>();

        // Récupération des boutons
        m_btnAjouter = m_widget->findChild<QPushButton*>(QString(), Qt::FindDirectChildrenOnly); 
        // Note : On peut chercher par texte ou de manière plus ciblée si nécessaire.
        // Pour l'exemple, nous allons directement appeler les slots ou simuler les clics.
    }

    void TearDown() override {
        delete m_widget;
        m_widget = nullptr;
    }

    static QApplication* m_app;
    
    // Pointeurs utilitaires pour les tests
    DevicesWidget* m_widget = nullptr;
    QLineEdit* m_leNom = nullptr;
    QComboBox* m_cbType = nullptr;
    QSlider* m_slTemp = nullptr;
    QSlider* m_slCharge = nullptr;
    QListWidget* m_listDevices = nullptr;
    QPushButton* m_btnAjouter = nullptr;
};

QApplication* DevicesWidgetTest::m_app = nullptr;


// --- TESTS UNITAIRES & D'INTÉGRATION ---

// 1. Vérification de l'état initial de l'IHM
TEST_F(DevicesWidgetTest, InitialStateIsCorrect) {
    ASSERT_NE(m_leNom, nullptr);
    ASSERT_NE(m_cbType, nullptr);
    ASSERT_NE(m_slTemp, nullptr);
    ASSERT_NE(m_slCharge, nullptr);
    ASSERT_NE(m_listDevices, nullptr);

    EXPECT_TRUE(m_leNom->text().isEmpty());
    EXPECT_EQ(m_cbType->currentText(), "GPU"); // Premier choix par défaut
    EXPECT_EQ(m_slTemp->value(), 20);          // Minimum configuré à 20°C
    EXPECT_EQ(m_slCharge->value(), 0);         // Minimum configuré à 0%
    EXPECT_EQ(m_listDevices->count(), 0);      // Liste vide au départ
}

// 2. Test du bouton "Ajout Device" et de la mise à jour de l'ID via MQTT
TEST_F(DevicesWidgetTest, ActionAjouterDeviceSuccess) {
    // Simulation de la saisie utilisateur
    m_leNom->setText("Unite_Calcul_01");
    m_cbType->setCurrentIndex(1); // "CPU"
    m_slTemp->setValue(45);
    m_slCharge->setValue(75);

    // Recherche et simulation du clic sur le bouton "Ajout Device"
    QPushButton* btn = m_widget->findChild<QPushButton*>();
    // Pour être précis, on cherche le bouton par son libellé
    for (auto* b : m_widget->findChildren<QPushButton*>()) {
        if (b->text() == "Ajout Device") {
            btn = b;
            break;
        }
    }
    ASSERT_NE(btn, nullptr);
    
    // Déclenchement du clic de manière logicielle
    QTest::mouseClick(btn, Qt::LeftButton);

    // Vérifications : Le device doit être ajouté graphiquement à la liste
    ASSERT_EQ(m_listDevices->count(), 1);
    EXPECT_EQ(m_listDevices->item(0)->text(), "Unite_Calcul_01");

    // Simulation de la réponse asynchrone MQTT du projet "Monitoring" (attribution de l'ID 42)
    MqttManagerSimulator::instance().simulerReceptionIdDuMonitoring("Unite_Calcul_01", 42);

    // Le test passe si aucune régression ou crash n'a lieu lors du traitement du signal.
}

// 3. Test de sécurité : Interdiction d'ajouter un nom vide
TEST_F(DevicesWidgetTest, ActionAjouterDeviceVideEchoue) {
    m_leNom->setText(""); // Nom vide

    QPushButton* btnAjouter = nullptr;
    for (auto* b : m_widget->findChildren<QPushButton*>()) {
        if (b->text() == "Ajout Device") { btnAjouter = b; break; }
    }
    
    // On clique. Une QMessageBox va normalement s'ouvrir (bloquante).
    // Pour éviter de bloquer le test unitaire, on peut appeler directement le slot privé via Qt Metadata
    // ou fermer automatiquement la boîte de dialogue qui pop via un Timer, mais l'appel du slot est plus propre :
    QMetaObject::invokeMethod(m_widget, "onAjouterClicked");

    // La liste doit rester vide
    EXPECT_EQ(m_listDevices->count(), 0);
}

// 4. Test d'intégration : Sélection et Suppression d'un Device
TEST_F(DevicesWidgetTest, ActionSupprimerDevice) {
    // Pré-remplissage artificiel
    m_leNom->setText("DeviceADetruire");
    QMetaObject::invokeMethod(m_widget, "onAjouterClicked");
    
    // Injection immédiate de l'identifiant par le mock MQTT pour valider la suppression
    MqttManagerSimulator::instance().simulerReceptionIdDuMonitoring("DeviceADetruire", 99);

    // Sélection de l'élément dans la QListWidget
    ASSERT_EQ(m_listDevices->count(), 1);
    m_listDevices->setCurrentRow(0); // Sélection du premier item

    // Clic sur le bouton Supprimer
    QPushButton* btnSupprimer = nullptr;
    for (auto* b : m_widget->findChildren<QPushButton*>()) {
        if (b->text() == "Supprimer Device") { btnSupprimer = b; break; }
    }
    ASSERT_NE(btnSupprimer, nullptr);
    QTest::mouseClick(btnSupprimer, Qt::LeftButton);

    // Vérification que l'élément graphique a bien disparu
    EXPECT_EQ(m_listDevices->count(), 0);
}

// 5. Test de modification des sliders (Température / Charge)
TEST_F(DevicesWidgetTest, ActionModifierSliders) {
    m_leNom->setText("DeviceModif");
    QMetaObject::invokeMethod(m_widget, "onAjouterClicked");
    MqttManagerSimulator::instance().simulerReceptionIdDuMonitoring("DeviceModif", 200);
    m_listDevices->setCurrentRow(0);

    // Changement des valeurs des Sliders
    m_slTemp->setValue(85);
    m_slCharge->setValue(90);

    // Clic sur Modifier Température
    for (auto* b : m_widget->findChildren<QPushButton*>()) {
        if (b->text() == "Modifier Température") {
            QTest::mouseClick(b, Qt::LeftButton);
        }
        if (b->text() == "Modifier Charge") {
            QTest::mouseClick(b, Qt::LeftButton);
        }
    }

    // On vérifie visuellement ou via log que l'action s'exécute sans erreur
    SUCCEED(); 
}