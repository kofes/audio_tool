cmake_minimum_required(VERSION 3.15)
project(audio_tool)


set(RESOURCES_PATH ${PROJECT_SOURCE_DIR}/res/)
configure_file(config.h.in ${PROJECT_SOURCE_DIR}/src/config.h @ONLY)

set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

find_package(SDL2 REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

find_package(SDL2_mixer REQUIRED)

add_executable(${PROJECT_NAME} src/player/player.cpp src/track/track.cpp src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE src/player src/track)

# string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

target_link_libraries(${PROJECT_NAME} SDL2::Mixer)