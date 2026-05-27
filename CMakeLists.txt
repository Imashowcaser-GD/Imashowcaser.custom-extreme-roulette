cmake_minimum_required(VERSION 3.24)

project(ExtremeCustomRoulette VERSION 1.0.0)

find_package(Geode REQUIRED)

add_geode_mod(
    ${PROJECT_NAME}
    ID imashowcaser.extreme_custom_roulette
    SOURCES 
        src/main.cpp
)
