cmake_minimum_required(VERSION 3.14)
project(SimulatorProject VERSION 1.0 LANGUAGES CXX)

# Configuration de C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuration de Qt (Automatique pour les Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


# Gestion de la librairie locale MqttComm
add_library(MqttComm STATIC IMPORTED GLOBALLY)
set_target_properties(MqttComm PROPERTIES
    IMPORTED_LOCATION "C:/Developpement informatique/Third-Party/mqtt-com/lib/MqttComm.lib"
    INTERFACE_INCLUDE_DIRECTORIES "C:/Developpement informatique/Third-Party/mqtt-com/include"
)

# Spécifier où CMake doit chercher les binaires de Qt (si non défini dans l'environnement)
# Note : on peut aussi ajouter d'autres path de librairies externes
set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/msvc2022_64") 

# Recherche des composants Qt requis (on utilise Core, Widgets et Gui)
find_package(Qt6 COMPONENTS Core Widgets Gui Test REQUIRED)

# nlohmann/json
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Inclusion du dossier src
add_subdirectory(src)