cmake_minimum_required(VERSION 3.16)
project(DevicesMqttProject VERSION 1.0.0 LANGUAGES CXX)

# --- Configuration C++ commune ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Configuration Qt commune (MOC requis pour Q_OBJECT / signaux-slots) ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF) # À passer à ON si des fichiers .ui sont ajoutés
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/msvc2022_64")
find_package(Qt6 COMPONENTS Core Widgets Gui Test REQUIRED)

# --- Répertoire Third-Party centralisé (au même niveau que ce projet) ---
get_filename_component(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Third-Party" ABSOLUTE)

# Boost
set(BOOST_ROOT "${THIRD_PARTY_DIR}/boost_1_73_0" CACHE PATH "Racine Boost")
# find_package(Boost REQUIRED COMPONENTS ...) # à activer/adapter selon les composants utilisés

# Paho MQTT (chemins réutilisés par MqttCom)
set(PAHO_MQTT_C_DIR   "${THIRD_PARTY_DIR}/paho-mqtt" CACHE PATH "Racine Paho C")
set(PAHO_MQTT_CPP_DIR "${THIRD_PARTY_DIR}/paho-mqtt" CACHE PATH "Racine Paho C++")
set(PAHO_MQTT_C_LIB   "${PAHO_MQTT_C_DIR}/lib/paho-mqtt3a-static.lib"   CACHE FILEPATH "Lib Paho C")
set(PAHO_MQTTPP_LIB   "${PAHO_MQTT_CPP_DIR}/lib/paho-mqttpp3-static.lib" CACHE FILEPATH "Lib Paho C++")

# GoogleTest (utilisé par MqttCom/src/Tests)
set(GTEST_DIR "${THIRD_PARTY_DIR}/googletest-main" CACHE PATH "Racine GoogleTest")

# --- nlohmann/json (partagé par les 3 sous-projets) ---
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# --- Options ---
option(BUILD_TESTS "Activer la compilation des tests" ON)

enable_testing()

# --- Sous-projets (ordre important : MqttCom d'abord, les autres en dépendent) ---
add_subdirectory(MqttCom)
add_subdirectory(Monitoring)
add_subdirectory(Simulator)