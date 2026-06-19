#pragma once
#include <vector>
#include <functional>
#include <iostream>

#include "Concept_1.h"

// Les types de notifications possibles (comme dans ton projet)
enum class Notifications: std::uint8_t {
    ItemAdded,
    ItemRemoved
};

template <typename T>
requires HasIntegralId<T>
class ObservableCollection {
private:
    std::vector<T> m_items;
    
    // Un "Callback" est maintenant une fonction qui prend un entier en paramètre (l'ID ou l'index)
    using Callback = std::function<void(int)>;

    // On stocke des paires : le type de notification et la fonction à appeler
    std::vector<std::pair<Notifications, Callback>> m_subscribers;

public:

    // PHASE 2 Un abonnement type-safe grâce aux concepts C++20
    template <QObjectDerived U, typename PointerToMemberFunction>
    bool subscribe(U* subscriber, PointerToMemberFunction memberFunction, Notifications notification) {
        // Pour l'Étape 3, on transforme le pointeur sur fonction membre en std::function
        // C'est ce qui lie le style "Qt" aux fonctions modernes C++
        m_subscribers.push_back({ notification, [subscriber, memberFunction](int handle) {
            (subscriber->*memberFunction)(handle);
        } });
        return true;
    }


    // Ajoute un élément et notifie les abonnés
    void add(const T& item) {
        m_items.push_back(item);
        
        // On récupère l'identifiant ou l'index du nouvel élément (ici l'ID de la station)
        int handle = item.id; 

        // Notification de l'événement ItemAdded
        notify(Notifications::ItemAdded, handle);
    }

    bool removeById(int id) {
        auto it = std::find_if(m_items.begin(), m_items.end(), [id](const T& item) {
            return item.id == id;
            });

        if (it != m_items.end()) {
            m_items.erase(it);

            // On déclenche l'événement ItemRemoved !
            notify(Notifications::ItemRemoved, id);
            return true;
        }
        return false; // ID non trouvé
    }

private:
    void notify(Notifications notification, int handle) {
        for (const auto& [type, callback] : m_subscribers) {
            if (type == notification) {
                callback(handle); // On appelle directement la fonction stockée !
            }
        }
    }
};