# On indique à CMake où se trouve le dossier Google Test extrait
set(GTEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/googletest-main")

# Options pour éviter que GTest ne crée des conflits de compilation sous Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# On demande à CMake d'inclure et de compiler Google Test à la volée
add_subdirectory(${GTEST_DIR} ${CMAKE_CURRENT_BINARY_DIR}/googletest_build)

# 2. Liste des fichiers de tests unitaires
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

# 3. Déclaration de l'exécutable de test
if (TEST_SOURCES)
    add_executable(Monitoring_Tests ${TEST_SOURCES}
        ../HardwareMonitoringEngine.cpp
        ../HardwareWidget.cpp
        ../include/HardwareDevice.h
        ../include/HardwareMonitoringEngine.h
        ../include/HardwareWidget.h)

    # Active le MOC pour votre binaire de test
    #set_target_properties(Monitoring_Tests PROPERTIES AUTOMOC ON)

    # 4. Liaisons obligatoires
    # On lie GTest, notre bibliothèque principale (MonitoringCore) et Qt6::Test pour tester l'UI si besoin
    target_link_libraries(Monitoring_Tests PRIVATE
        gtest
        gtest_main
        MonitoringCore
        Qt6::Core      # Requis pour les signaux de base
        Qt6::Widgets   # Requis pour QApplication, QPushButton, etc. presents dans les tests
        Qt6::Test
    )

    # 5. Découverte automatique des tests par CMake (permet de faire "ctest")
    include(GoogleTest)
    gtest_discover_tests(Monitoring_Tests)

endif()