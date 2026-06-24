cmake_minimum_required(VERSION 3.14)
project(MqttComProject VERSION 1.0.0 LANGUAGES CXX)

# Standard C++ (Recommandé C++20 si tu utilises std::jthread)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# nlohmann/json
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

## --- CONFIGURATION DES CHEMINS PAHO (À adapter si besoin) ---
set(PAHO_MQTT_C_DIR "C:/Developpement informatique/Third-Party/paho-mqtt" CACHE PATH "Racine Paho C")
set(PAHO_MQTT_CPP_DIR "C:/Developpement informatique/Third-Party/paho-mqtt" CACHE PATH "Racine Paho C++")

# Chemins absolus vers les fichiers .lib générés précédemment
set(PAHO_MQTT_C_LIB "C:/Developpement informatique/Third-Party/paho-mqtt/lib/paho-mqtt3a-static.lib" CACHE FILEPATH "Lib Paho C")
set(PAHO_MQTTPP_LIB "C:/Developpement informatique/Third-Party/paho-mqtt/lib/paho-mqttpp3-static.lib" CACHE FILEPATH "Lib Paho C++")

## --- OPTIONS ---
option(BUILD_TESTS "Activer la compilation des tests" ON)

# ABOLUMENT REQUIS pour réveiller l'Explorateur de Tests sous CMake
enable_testing()

## --- INCLUSION DES SOURCES ---
# On bascule dans le répertoire src/
add_subdirectory(src)