cmake_minimum_required(VERSION 3.13.4)

# Configura el SDK de Geode antes de buscar el paquete
include($ENV{GEODE_SDK}/cmake/link.cmake)

project(ExtremeCustomRoulette)

find_package(Geode REQUIRED)

add_geode_mod(
    lmashowcaser.custom-extreme-roulette
    "1.0.0"
    src/main.cpp
)

