cmake_minimum_required(VERSION 3.15)
project(audio_tool)

set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

add_executable(${PROJECT_NAME} src/player/player.cpp src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE src/player)