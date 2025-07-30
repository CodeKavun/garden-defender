cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 23)
project(GardenDefender)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/game/)

add_subdirectory(external/raylib)
add_subdirectory(external/json)
add_subdirectory(game)