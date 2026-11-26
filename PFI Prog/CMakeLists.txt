
cmake_minimum_required(VERSION 3.7.2)

project(PFI_Programmation_Temps_Reel LANGUAGES CXX)

# ========================================
# Configuration du compilateur C++17
# Compatible avec CMake 3.7.2
# ========================================

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    message(FATAL_ERROR
        "Ce projet demande GCC ou Clang avec C++17 sur Linux, WSL ou Raspberry Pi."
    )
endif()

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag(
    "-std=c++17"
    COMPILER_SUPPORTS_CXX17
)

if(NOT COMPILER_SUPPORTS_CXX17)
    message(FATAL_ERROR
        "Compilateur trop ancien : C++17 non pris en charge."
    )
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# ========================================
# Bibliotheques systeme
# ========================================

find_package(Threads REQUIRED)

find_path(
    MOSQUITTO_INCLUDE_DIR
    NAMES mosquitto.h
)

find_library(
    MOSQUITTO_LIB
    NAMES mosquitto
)

if(NOT MOSQUITTO_INCLUDE_DIR OR NOT MOSQUITTO_LIB)
    message(FATAL_ERROR
        "Bibliotheque Mosquitto introuvable. Installer libmosquitto-dev."
    )
endif()

# ========================================
# Bibliotheque du professeur
# ========================================

add_library(bibliotheque_panneau
    bibliotheque/Panneau.cpp
    bibliotheque/SerialPort.cpp
    bibliotheque/Mutex.cpp
    bibliotheque/Thread.cpp
)

target_include_directories(bibliotheque_panneau PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/bibliotheque
)

target_link_libraries(bibliotheque_panneau PUBLIC
    Threads::Threads
)

# ========================================
# Interface du panneau RAM
# ========================================

add_library(interface_panneau
    src/panneau/InterfacePanneau.cpp
)

target_include_directories(interface_panneau PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(interface_panneau PUBLIC
    bibliotheque_panneau
)

# ========================================
# Controle du panneau RAM
# ========================================

add_library(controle_panneau
    src/panneau/ControlePanneau.cpp
)

target_include_directories(controle_panneau PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ========================================
# Client MQTT
# ========================================

add_library(client_mqtt
    src/commun/ClientMqtt.cpp
)

target_include_directories(client_mqtt PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MOSQUITTO_INCLUDE_DIR}
)

target_link_libraries(client_mqtt PUBLIC
    ${MOSQUITTO_LIB}
)

# ========================================
# Regulation PID des bassins
# ========================================

add_library(regulation_bassins
    src/panneau/Pid.cpp
    src/panneau/RegulationBassins.cpp
)

target_include_directories(regulation_bassins PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ========================================
# Programme principal du panneau RAM
# ========================================

add_executable(panneau_ram
    src/panneau/main.cpp
    src/panneau/GestionAlarmes.cpp
)

target_link_libraries(panneau_ram PRIVATE
    interface_panneau
    client_mqtt
    controle_panneau
    regulation_bassins
    Threads::Threads
)

# ========================================
# Tests unitaires
# ========================================

include(CTest)

if(BUILD_TESTING)

    # Test du calcul PID

    add_executable(test_pid_unitaire
        tests/test_pid.cpp
    )

    target_link_libraries(test_pid_unitaire PRIVATE
        regulation_bassins
    )

    add_test(
        NAME pid_unitaire
        COMMAND test_pid_unitaire
    )

    # Test de la regulation des bassins

    add_executable(test_regulation_unitaire
        tests/test_regulation_bassins.cpp
    )

    target_link_libraries(test_regulation_unitaire PRIVATE
        regulation_bassins
    )

    add_test(
        NAME regulation_unitaire
        COMMAND test_regulation_unitaire
    )

    # Test de la regulation avec les purges

    add_executable(test_regulation_purge
        tests/test_regulation_purge.cpp
        src/panneau/GestionAlarmes.cpp
    )

    target_link_libraries(test_regulation_purge PRIVATE
        regulation_bassins
    )

    add_test(
        NAME regulation_purge
        COMMAND test_regulation_purge
    )

    # Test de la derivee du PID

    add_executable(test_pid_derivation
        tests/test_pid_derivation.cpp
    )

    target_link_libraries(test_pid_derivation PRIVATE
        regulation_bassins
    )

    add_test(
        NAME pid_derivation
        COMMAND test_pid_derivation
    )

endif()
