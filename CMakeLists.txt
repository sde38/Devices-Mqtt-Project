cmake_minimum_required(VERSION 3.16)

# Nom du projet
project(Monitoring VERSION 1.0.0 LANGUAGES CXX)

# Activation obligatoire du standard C++20 pour les Concepts et std::jthread
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuration automatique de Qt (MOC est requis car on utilise Q_OBJECT et les signaux/slots)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF) # À passer à ON si tu ajoutes des fichiers .ui plus tard
set(CMAKE_AUTORCC ON)

# Spécifier où CMake doit chercher les binaires de Qt (si non défini dans l'environnement)
# Note : on peut aussi ajouter d'autres path de librairies externes
set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/msvc2022_64") 

# Recherche des composants Qt requis (on utilise Core, Widgets et Gui)
find_package(Qt6 COMPONENTS Core Widgets Gui Test REQUIRED)

# ABOLUMENT REQUIS pour réveiller l'Explorateur de Tests sous CMake
enable_testing()

# Indique à CMake d'aller lire le fichier de configuration dans le dossier src
add_subdirectory(src)