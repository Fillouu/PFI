# DEBUT DU CODE GENERE AVEC L'AIDE D'UNE IA
cmake_minimum_required(VERSION 3.16)
project(PFI_Programmation_Temps_Reel VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Le squelette compile sans Raspberry Pi ni bibliotheque MQTT.
# Les adaptateurs fournis ne dialoguent PAS encore avec le materiel.
add_library(commun src/commun/ClientMqtt.cpp)
target_include_directories(commun PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(panneau
    src/panneau/InterfacePanneau.cpp
    src/panneau/Pid.cpp
    src/panneau/GestionAlarmes.cpp
    src/panneau/ControlePanneau.cpp
)
target_link_libraries(panneau PUBLIC commun)
target_include_directories(panneau PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(powermeter
    src/powermeter/LecteurPowerMeter.cpp
    src/powermeter/ControlePowerMeter.cpp
)
target_link_libraries(powermeter PUBLIC commun)
target_include_directories(powermeter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(panneau_ram src/panneau/main.cpp)
target_link_libraries(panneau_ram PRIVATE panneau)

add_executable(power_meter src/powermeter/main.cpp)
target_link_libraries(power_meter PRIVATE powermeter)

enable_testing()
add_executable(test_pid tests/test_pid.cpp)
target_link_libraries(test_pid PRIVATE panneau)
add_test(NAME PID_unitaire COMMAND test_pid)
# FIN DU CODE GENERE AVEC L'AIDE D'UNE IA
