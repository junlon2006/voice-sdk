cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(VUI LANGUAGES C)

add_subdirectory("src")
add_subdirectory("src/audioin")
add_subdirectory("src/aec")
add_subdirectory("src/recognize")
