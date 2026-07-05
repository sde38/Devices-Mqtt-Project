cmake_minimum_required(VERSION 3.14)

# Création d'une cible contenant la logique pour pouvoir la réutiliser dans les tests
add_library(SimulatorLib OBJECT
    include/MainWindows.h
    include/DevicesWidget.h
    include/MqttManagerSimulator.h
    MainWindows.cpp
    DevicesWidget.cpp
)

target_include_directories(SimulatorLib PUBLIC include)

target_link_libraries(SimulatorLib PUBLIC 
    Qt6::Widgets 
    MqttComm
    nlohmann_json::nlohmann_json)

# Création de l'exécutable final de l'application
add_executable(Simulator Main.cpp)

target_include_directories(Simulator PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(Simulator PRIVATE SimulatorLib)

# nlohmann_json est déjà propagé via SimulatorLib PUBLIC → ligne redondante supprimée

if(BUILD_TESTS)
    add_subdirectory(Tests)
endif()