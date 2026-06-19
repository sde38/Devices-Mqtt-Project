// Ce fichier déclare la classe du moteur. Il intègre un mécanisme pour mettre à jour les données 
// en arrière-plan sans bloquer l'application. 
// On utilise un std::jthread (C++20) pour simuler l'arrivée et le départ de périphériques.

// Pour la couche métier, le HardwareMonitoringEngine est le chef d'orchestre de ton application. 
// C'est lui qui possède et gère la ObservableCollection, et qui simule (ou récupère) l'activité 
// du matériel informatique.

#pragma once
#include <thread>
#include <atomic>
#include "ObservableCollection.h"
#include "HardwareDevice.h"

class HardwareMonitoringEngine {
private:
    // La collection réactive qui contient nos périphériques
    ObservableCollection<HardwareDevice> m_devices;

    // Gestion du thread de simulation arrière-plan
    std::jthread m_simulationThread;
    std::atomic<bool> m_running{false};
    int m_nextId{1}; // Compteur pour générer des ID uniques

    // Fonction interne exécutée par le thread
    void runSimulation();

public:
    HardwareMonitoringEngine();
    ~HardwareMonitoringEngine();

    // Permet de démarrer et arrêter la surveillance/simulation
    void start();
    void stop();

    // Donne un accès en lecture seule à la collection pour que l'UI puisse s'y abonner
    ObservableCollection<HardwareDevice>& getDevices() { return m_devices; }
    
    // Fonction utilitaire pour forcer manuellement la suppression d'un composant (ex: via l'UI)
    void simulateDisconnection(int id);
};