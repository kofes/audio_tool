cmake_minimum_required(VERSION 3.15)
project(audio_tool)

set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

find_package(SDL2 REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} src/player/player.cpp src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE src/player)

string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
