# Définition des fichiers de la bibliothèque
set(LIB_SOURCES
    MqttManager.cpp
    include/MqttManager.h
)

# Création de la bibliothèque statique
add_library(MqttComm STATIC ${LIB_SOURCES})

# Configuration des répertoires d'inclusion
# PUBLIC permet aux fichiers de Tests et aux futures apps d'accéder à MqttManager.h automatiquement
target_include_directories(MqttComm PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    "${PAHO_MQTT_C_DIR}/src"
    "${PAHO_MQTT_CPP_DIR}/include"
)

# Lien avec la dépendance JSON
target_link_libraries(MqttComm PUBLIC nlohmann_json::nlohmann_json)

# Définitions obligatoires pour le mode statique de Paho (C et C++)
target_compile_definitions(MqttComm PUBLIC
    PAHO_MQTTPP_STATIC
    PAHO_MQTT_STATIC
)

# PUBLIC : propage automatiquement les .lib Paho à quiconque link MqttComm
# (Tests, Monitoring, Simulator n'ont plus besoin de les relier explicitement)
target_link_libraries(MqttComm PUBLIC
    "${PAHO_MQTTPP_LIB}"
    "${PAHO_MQTT_C_LIB}"
)

# Spécificités Windows (Sockets et Warning d'anciennes fonctions C)
if(WIN32)
    target_compile_definitions(MqttComm PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_link_libraries(MqttComm INTERFACE ws2_32)
endif()

## --- INCLUSION DES TESTS ---
if(BUILD_TESTS)
    add_subdirectory(Tests)
endif()