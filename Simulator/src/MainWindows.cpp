#include "MainWindows.h"

MainWindows::MainWindows(QWidget *parent) : QMainWindow(parent) {
    // Définition du titre et de la taille initiale de la fenêtre
    setWindowTitle("Simulateur de Devices (Simulator Project)");
    resize(800, 400);

    // Initialisation du Widget central
    DevicesWidget* centralWidget = new DevicesWidget(this);
    setCentralWidget(centralWidget);
}