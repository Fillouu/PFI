# Systeme cible
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armv7)

# Compilateurs croises
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

# Architecture Raspberry Pi 2
set(ARM_FLAGS "-mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard")

# Options de compilation
set(CMAKE_C_FLAGS_INIT "${ARM_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${ARM_FLAGS}")

# Liaison statique
set(CMAKE_EXE_LINKER_FLAGS_INIT "-static")

# Bibliotheques ARM
set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)

# Recherche des dependances
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
