# GTEST_DIR est hérité du CMakeLists racine (Devices Mqtt Project) s'il est défini,
# sinon (build standalone de Monitoring) on le recalcule.
if(NOT DEFINED GTEST_DIR)
    get_filename_component(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Third-Party" ABSOLUTE)
    set(GTEST_DIR "${THIRD_PARTY_DIR}/googletest-main" CACHE PATH "Racine GoogleTest")
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# Protection contre le double-import de GoogleTest (déjà importé par MqttCom/src/Tests
# dans le build fusionné)
if(NOT TARGET gtest)
    add_subdirectory(${GTEST_DIR} ${CMAKE_CURRENT_BINARY_DIR}/googletest_build)
endif()

# Liste des fichiers de tests unitaires
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

if (TEST_SOURCES)
    add_executable(Monitoring_Tests ${TEST_SOURCES}
        ../DevicesMonitoringEngine.cpp
        ../DeviceWidget.cpp
        ../MqttManagerMonitoring.cpp
        ../include/DevicesMonitoringEngine.h
        ../include/DeviceWidget.h
        ../include/MqttManagerMonitoring.h)

    #set_target_properties(Monitoring_Tests PROPERTIES AUTOMOC ON)

    target_link_libraries(Monitoring_Tests PRIVATE
        gtest
        gtest_main
        MonitoringCore
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
    )

    include(GoogleTest)
    gtest_discover_tests(Monitoring_Tests)
endif()