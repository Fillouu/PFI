cmake_minimum_required(VERSION 3.16)

project(PFI_Programmation_Temps_Reel LANGUAGES CXX)

# Configuration C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# ========================================
# Interface du panneau RAM
# ========================================

add_library(interface_panneau
    src/panneau/InterfacePanneau.cpp
)

add_library(controle_panneau
    src/panneau/ControlePanneau.cpp
)

target_include_directories(controle_panneau PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(interface_panneau PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(interface_panneau PUBLIC
    bibliotheque_panneau
)

# ========================================
# Bibliotheque MQTT - Mosquitto
# ========================================

find_path(MOSQUITTO_INCLUDE_DIR mosquitto.h REQUIRED)
find_library(MOSQUITTO_LIB mosquitto REQUIRED)

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
# Programme principal - Panneau RAM
# ========================================

add_executable(panneau_ram
    src/panneau/main.cpp
)

target_link_libraries(panneau_ram PRIVATE
    interface_panneau
    client_mqtt
    controle_panneau
)