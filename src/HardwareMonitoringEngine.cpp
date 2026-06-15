// C'est ici qu'on écrit la logique. Pour que ton projet soit vivant et visuel, la fonction runSimulation 
// va ajouter un composant (ex: un CPU, une clé USB) toutes les quelques secondes, puis en supprimer 
// un de temps en temps.

// Point d'attention pour l'étape Qt (Le Multi-threading)
// Ici, la méthode m_devices.add() va être appelée depuis le thread secondaire (m_simulationThread).
// Quand le callback de ton concept va s'exécuter, il va appeler directement la fonction de ta MainWindow Qt depuis ce thread secondaire.

// Règle d'or de Qt : On ne doit jamais modifier l'interface graphique depuis un autre thread que le thread principal.

// Pour résoudre cela très simplement dans la couche UI (MainWindow), il te suffira d'utiliser QMetaObject::invokeMethod dans ta fonction de 
// callback. Cela permettra de renvoyer l'ordre d'affichage sur le thread principal en toute sécurité !

#include "HardwareMonitoringEngine.h"
#include <chrono>

HardwareMonitorEngine::HardwareMonitorEngine() {
    // On commence avec quelques périphériques de base fixes au démarrage
    m_devices.add(HardwareDevice{m_nextId++, "Intel Core i9-13900K", "CPU", 42.5f, 12.0f});
    m_devices.add(HardwareDevice{m_nextId++, "NVIDIA RTX 4090", "GPU", 35.0f, 0.0f});
}

HardwareMonitorEngine::~HardwareMonitorEngine() {
    stop();
}

void HardwareMonitorEngine::start() {
    if (m_running) return;
    m_running = true;
    
    // Lancement du thread C++20 (m_simulationThread s'occupe de sa propre durée de vie)
    m_simulationThread = std::jthread(&HardwareMonitorEngine::runSimulation, this);
}

void HardwareMonitorEngine::stop() {
    m_running = false;
    if (m_simulationThread.joinable()) {
        m_simulationThread.join();
    }
}

void HardwareMonitorEngine::simulateDisconnection(int id) {
    // Utilise la méthode removeById que l'on a planifiée pour ObservableCollection
    m_devices.removeById(id);
}

void HardwareMonitorEngine::runSimulation() {
    int loopCount = 0;

    while (m_running) {
        // On attend 3 secondes entre chaque événement de simulation
        std::this_thread::sleep_for(std::chrono::seconds(3));
        if (!m_running) break;

        loopCount++;

        // Tous les 3 cycles, on simule l'insertion d'un nouveau périphérique
        if (loopCount % 3 == 0) {
            HardwareDevice newDrive{
                m_nextId++, 
                "Clé USB SanDisk (Disque " + std::to_string(m_nextId) + ")", 
                "Stockage", 
                28.0f, 
                0.0f
            };
            
            // Cela va notifier automatiquement la MainWindow grâce à ton concept !
            m_devices.add(newDrive); 
        }

        // Tous les 7 cycles, on simule une déconnexion surprise du périphérique avec l'ID 3
        if (loopCount % 7 == 0) {
            m_devices.removeById(3); 
        }
    }
}