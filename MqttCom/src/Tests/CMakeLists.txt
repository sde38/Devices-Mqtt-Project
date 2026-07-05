# GTEST_DIR est défini soit par le CMakeLists racine (Devices Mqtt Project),
# soit par le fallback dans MqttCom/CMakeLists.txt en build standalone.

# Application des options spécifiques de compilation
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# Inclusion et compilation à la volée dans un dossier de build dédié
add_subdirectory(${GTEST_DIR} ${CMAKE_CURRENT_BINARY_DIR}/googletest_build)

# Création de l'exécutable de test
add_executable(Mqtt_Tests MqttManager_UT.cpp)

# Liaison des dépendances
# MqttComm étant PUBLIC sur Paho et json, tout est propagé automatiquement.
target_link_libraries(Mqtt_Tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    MqttComm
)

# Active l'affichage des dossiers dans Visual Studio pour les tests
set_target_properties(Mqtt_Tests PROPERTIES FOLDER "Tests")

# Découverte automatique des tests par CMake (permet de faire "ctest")
include(GoogleTest)
gtest_discover_tests(Mqtt_Tests)