# 1. Liste des fichiers sources partagés
set(CORE_SOURCES
    HardwareMonitoringEngine.cpp
    HardwareWidget.cpp
    MainWindow.cpp
)

# 2. On crée une bibliothèque statique contenant la logique de l'application
add_library(MonitoringCore STATIC ${CORE_SOURCES}
    include/HardwareMonitoringEngine.h
    include/HardwareWidget.h
    include/MainWindow.h)

target_include_directories(MonitoringCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(MonitoringCore PUBLIC Qt6::Core Qt6::Widgets Qt6::Gui)

# 3. L'application principale devient ultra simple : elle utilise juste main.cpp et la bibliothèque
add_executable(Monitoring 
    main.cpp 
)

target_link_libraries(Monitoring PRIVATE MonitoringCore)

# 4. On indique à CMake d'aller lire le dossier de Tests
add_subdirectory(Tests)
