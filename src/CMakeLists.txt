cmake_minimum_required(VERSION 3.14)

# Création d'une cible contenant la logique pour pouvoir la réutiliser dans les tests
add_library(SimulatorLib OBJECT
    include/MainWindows.h
    include/DevicesWidget.h
    include/MqttManagerSimulator.h
    MainWindows.cpp
    DevicesWidget.cpp
)

# Configuration des dossiers d'inclusion
target_include_directories(SimulatorLib PUBLIC include)
 
target_link_libraries(SimulatorLib PUBLIC 
    Qt6::Widgets MqttComm
    nlohmann_json::nlohmann_json)

# Création de l'exécutable final de l'application
add_executable(Simulator Main.cpp)

target_include_directories(Simulator PUBLIC 
 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(Simulator PRIVATE SimulatorLib)

# Lien avec la dépendance JSON
target_link_libraries(Simulator PUBLIC nlohmann_json::nlohmann_json)

# 3. Inclusion des tests
add_subdirectory(Tests)