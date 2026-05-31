cmake_minimum_required(VERSION 3.13.4)

project(ExtremeCustomRoulette)

# GitHub y tu PC encontrarán Geode automáticamente aquí sin usar variables de entorno manuales
find_package(Geode REQUIRED)

add_geode_mod(
    imashowcaser.extreme_custom_roulette
    "1.0.0"
    src/main.cpp
)
