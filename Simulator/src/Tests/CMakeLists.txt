cmake_minimum_required(VERSION 3.14)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# GTEST_SOURCE_DIR : hérité du CMakeLists racine (Devices Mqtt Project) via GTEST_DIR si présent,
# sinon (build standalone de Simulator) recalculé depuis Third-Party.
if(DEFINED GTEST_DIR)
    set(GTEST_SOURCE_DIR "${GTEST_DIR}")
else()
    get_filename_component(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Third-Party" ABSOLUTE)
    set(GTEST_SOURCE_DIR "${THIRD_PARTY_DIR}/googletest-main")
endif()
set(GTEST_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/googletest-build")

# Protection contre le double-import de GoogleTest (déjà importé par MqttCom/src/Tests
# ou Monitoring/src/Tests dans le build fusionné)
if(NOT TARGET gtest)
    add_subdirectory(${GTEST_SOURCE_DIR} ${GTEST_BINARY_DIR})
endif()

include(GoogleTest)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(SimulatorTests ${TEST_SOURCES})

target_link_libraries(SimulatorTests PRIVATE
    SimulatorLib
    gtest
    gtest_main
    Qt6::Widgets
    Qt6::Test
)

target_include_directories(SimulatorTests PRIVATE ../include)

gtest_discover_tests(SimulatorTests)