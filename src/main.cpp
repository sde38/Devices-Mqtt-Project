// Pour lier le tout, le point d'entrée instancie le moteur, démarre la boucle de simulation, 
// affiche la fenêtre et démarre l'application.

#include <QApplication>
#include "HardwareMonitoringEngine.h"
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 1. Instanciation du moteur métier
    HardwareMonitoringEngine engine;
    
    // 2. Création de la fenêtre Qt (qui va s'abonner au moteur)
    MainWindow window(engine);
    window.show();

    // 3. Démarrage de la simulation d'arrière-plan (thread C++20)
    engine.start();

    int result = app.exec();

    // 4. Arrêt propre du thread à la fermeture de l'UI
    engine.stop(); 
    
    return result;
}