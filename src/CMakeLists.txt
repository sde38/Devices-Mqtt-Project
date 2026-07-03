# Liste des fichiers sources partagés
set(CORE_SOURCES
    DevicesMonitoringEngine.cpp
    DeviceWidget.cpp
    MainWindow.cpp
    MqttManagerMonitoring.cpp
)

# Création d'une bibliothèque statique contenant la logique de l'application
add_library(MonitoringCore STATIC ${CORE_SOURCES}
    include/DevicesMonitoringEngine.h
    include/DeviceWidget.h
    include/MainWindow.h
    include/MqttManagerMonitoring.h)

target_include_directories(MonitoringCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ws2_32 est déjà propagé par MqttComm (INTERFACE sur WIN32) → plus besoin de le relier ici
target_link_libraries(MonitoringCore PUBLIC 
    Qt6::Core 
    Qt6::Widgets
    Qt6::Gui 
    MqttComm
    nlohmann_json::nlohmann_json)

# L'application principale utilise juste main.cpp et la bibliothèque
add_executable(Monitoring main.cpp)

target_link_libraries(Monitoring PRIVATE MonitoringCore)

# Lire le dossier de Tests
if(BUILD_TESTS)
    add_subdirectory(Tests)
endif()