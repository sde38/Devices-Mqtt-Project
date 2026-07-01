cmake_minimum_required(VERSION 3.14)

# --- Configuration locale de GoogleTest ---

# Options demandées
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# Chemin local vers GoogleTest (par rapport à ce fichier CMakeLists.txt)
# ./src/Tests/ -> ../../../Third-Party/googletest-main
set(GTEST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../Third-Party/googletest-main")
set(GTEST_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/googletest-build")

# Ajout du sous-répertoire avec spécification du dossier de build binaire
add_subdirectory(${GTEST_SOURCE_DIR} ${GTEST_BINARY_DIR})

# Inclusion des macros de découverte de tests
include(GoogleTest)

# --- Configuration de l'exécutable de Tests ---

# Récupération automatique de tous les fichiers de tests présents dans le dossier
file(GLOB TEST_SOURCES "*.cpp")

add_executable(SimulatorTests ${TEST_SOURCES})


# Liens avec l'application, GTest et Qt
target_link_libraries(SimulatorTests PRIVATE
    SimulatorLib          # Contient MainWindows.o et DevicesWidget.o
    gtest
    gtest_main
    Qt6::Widgets          # Nécessaire si les widgets sont testés
    Qt6::Test
)

# Spécification des dossiers d'inclusion pour trouver les headers du projet (MainWindows.h...)
target_include_directories(SimulatorTests PRIVATE ../include)

# Découverte automatique des tests par CMake (Ctest)
gtest_discover_tests(SimulatorTests)