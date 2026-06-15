// Tester de l'interface graphique Qt nécessite d'instancier une QApplication éphémère 
// (pour éviter les crashs d'affichage système) et utilise le module QtTest.

#include <gtest/gtest.h>
#include <QApplication>
#include <QTest>
#include <QSignalSpy>

#include "HardwareWidget.h"

// Fixture permettant de configurer l'environnement graphique Qt avant chaque test
class HardwareWidgetTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // GTest a besoin d'une instance QApplication pour instancier des QWidgets
        static int argc = 1;
        static char* argv[] = {(char*)"executable"};
        static QApplication app(argc, argv);
    }
};

TEST_F(HardwareWidgetTest, WidgetInitializationAndDataUpdate) {
    HardwareDevice device{12, "Fake Fan", "COOLER", 30.0f, 50.0f};
    HardwareWidget widget(device);

    // On vérifie que le widget a bien capturé l'ID
    EXPECT_EQ(widget.getDeviceId(), 12);

    // On simule une mise à jour des données du composant graphique
    widget.updateData(45.0f, 85.0f);
    
    // Le test passe si aucune erreur interne ou crash de l'UI Qt ne survient
    SUCCEED();
}

TEST_F(HardwareWidgetTest, ButtonClickEmitsSignal) {
    HardwareDevice device{77, "Removable Disk", "USB", 25.0f, 0.0f};
    HardwareWidget widget(device);

    // Utilisation du composant QtTest pour intercepter les signaux
    QSignalSpy spy(&widget, &HardwareWidget::disconnectionRequested);

    // On cherche le bouton "Simuler Déconnexion" dans les enfants du Widget
    QPushButton* button = widget.findChild<QPushButton*>();
    ASSERT_NE(button, nullptr);

    // On simule un clic physique de souris sur le bouton avec Qt Test
    QTest::mouseClick(button, Qt::LeftButton);

    // On vérifie qu'un signal a bien été émis
    EXPECT_EQ(spy.count(), 1);
    
    // On vérifie que le premier argument envoyé par le signal est bien l'ID 77
    QList<QVariant> arguments = spy.takeFirst();
    EXPECT_EQ(arguments.at(0).toInt(), 77);
}