cmake_minimum_required(VERSION 3.14)
project(MqttComProject VERSION 1.0.0 LANGUAGES CXX)

# Standard C++ (utile uniquement en build standalone ;
# hérité du CMakeLists racine si intégré au projet global)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# nlohmann/json — protégé pour éviter un doublon si déjà déclaré par le projet racine
if(NOT TARGET nlohmann_json::nlohmann_json)
    include(FetchContent)
    FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
    FetchContent_MakeAvailable(json)
endif()

## --- CHEMINS THIRD-PARTY ---
# Si le projet racine (Devices Mqtt Project) a déjà défini ces variables, on les réutilise.
# Sinon (build standalone de MqttCom), on les recalcule depuis Third-Party.
if(NOT DEFINED PAHO_MQTT_C_DIR)
    get_filename_component(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Third-Party" ABSOLUTE)
    set(PAHO_MQTT_C_DIR   "${THIRD_PARTY_DIR}/paho-mqtt" CACHE PATH "Racine Paho C")
    set(PAHO_MQTT_CPP_DIR "${THIRD_PARTY_DIR}/paho-mqtt" CACHE PATH "Racine Paho C++")
    set(PAHO_MQTT_C_LIB   "${PAHO_MQTT_C_DIR}/lib/paho-mqtt3a-static.lib"   CACHE FILEPATH "Lib Paho C")
    set(PAHO_MQTTPP_LIB   "${PAHO_MQTT_CPP_DIR}/lib/paho-mqttpp3-static.lib" CACHE FILEPATH "Lib Paho C++")
    set(GTEST_DIR         "${THIRD_PARTY_DIR}/googletest-main" CACHE PATH "Racine GoogleTest")
endif()

## --- OPTIONS ---
option(BUILD_TESTS "Activer la compilation des tests" ON)

# ABSOLUMENT REQUIS pour réveiller l'Explorateur de Tests sous CMake
enable_testing()

## --- INCLUSION DES SOURCES ---
add_subdirectory(src)