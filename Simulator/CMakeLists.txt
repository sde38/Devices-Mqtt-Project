cmake_minimum_required(VERSION 3.14)
project(SimulatorProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

# MqttComm n'est plus une lib IMPORTED : c'est le vrai target compilé par MqttCom.
# Réutilise-le si déjà présent (build fusionné), sinon (standalone) compile MqttCom depuis les sources.
if(NOT TARGET MqttComm)
    add_subdirectory(
        "${CMAKE_CURRENT_SOURCE_DIR}/../MqttCom"
        "${CMAKE_CURRENT_BINARY_DIR}/MqttCom_build"
    )
endif()

# Qt : réutilise la config racine si déjà résolue, sinon la configure ici
if(NOT TARGET Qt6::Core)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/msvc2022_64")
    find_package(Qt6 COMPONENTS Core Widgets Gui Test REQUIRED)
endif()

# nlohmann/json — protégé contre le doublon
if(NOT TARGET nlohmann_json::nlohmann_json)
    include(FetchContent)
    FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
    FetchContent_MakeAvailable(json)
endif()

option(BUILD_TESTS "Activer la compilation des tests" ON)
enable_testing()

add_subdirectory(src)