cmake_minimum_required(VERSION 3.14)

# Définition du chemin absolu/relatif vers le dossier GoogleTest partagé
set(GTEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/googletest-main")

# Application de vos options spécifiques de compilation
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# Inclusion et compilation à la volée dans un dossier de build dédié
add_subdirectory(${GTEST_DIR} ${CMAKE_CURRENT_BINARY_DIR}/googletest_build)

# Création de l'exécutable de test
add_executable(Mqtt_Tests MqttManager_UT.cpp)


# Liaison des dépendances
# Grâce au mécanisme de CMake, lier MqttComm va propager automatiquement 
# les répertoires d'inclusion (include) et la macro PAHO_MQTTPP_STATIC à l'exécutable de test.
target_link_libraries(Mqtt_Tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    MqttComm
    "${PAHO_MQTTPP_LIB}"
    "${PAHO_MQTT_C_LIB}"
)

# Optionnel : Active l'affichage des dossiers dans Visual Studio pour les tests
set_target_properties(Mqtt_Tests PROPERTIES FOLDER "Tests")

# Découverte automatique des tests par CMake (permet de faire "ctest")
include(GoogleTest)
gtest_discover_tests(Mqtt_Tests)