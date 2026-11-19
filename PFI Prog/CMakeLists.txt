
# DEBUT DU CODE GENERE AVEC L'AIDE D'UNE IA

cmake_minimum_required(VERSION 3.16)

project(PFI_Programmation_Temps_Reel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Bibliotheque du professeur

add_library(bibliotheque_panneau
    bibliotheque/Panneau.cpp
    bibliotheque/SerialPort.cpp
    bibliotheque/Mutex.cpp
    bibliotheque/Thread.cpp
)


target_include_directories(bibliotheque_panneau PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/bibliotheque
)

# Notre interface panneau
add_library(interface_panneau
    src/panneau/InterfacePanneau.cpp
)

target_include_directories(interface_panneau PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(interface_panneau PUBLIC
    bibliotheque_panneau
)

# Programme principal
add_executable(panneau_ram
    src/panneau/main.cpp
)

target_link_libraries(panneau_ram PRIVATE
    interface_panneau
)

# FIN DU CODE GENERE AVEC L'AIDE D'UNE IA
