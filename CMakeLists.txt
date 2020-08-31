# version required
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

# configure information
project(sampleVoiceSdk LANGUAGES C)

add_subdirectory("app")
add_subdirectory("hal")
add_subdirectory("sdk")
add_subdirectory("utils")
