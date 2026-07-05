cmake_minimum_required(VERSION 3.16)
project(Monitoring VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

# MqttComm n'est plus une lib IMPORTED : elle est fournie par le vrai target
# compilé dans MqttCom (add_subdirectory(MqttCom) appelé en amont depuis la racine).
# → Le bloc add_library(MqttComm STATIC IMPORTED GLOBALLY) est supprimé.

# Qt : réutilise la config du CMakeLists racine si déjà résolue,
# sinon (build standalone) la configure ici.
if(NOT TARGET Qt6::Core)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/msvc2022_64")
    find_package(Qt6 COMPONENTS Core Widgets Gui Test REQUIRED)
endif()

# nlohmann/json — protégé contre le doublon avec le CMakeLists racine
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