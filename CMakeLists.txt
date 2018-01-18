cmake_minimum_required(VERSION 3.0)
project(evdev-calibration)
set(CMAKE_CXX_STANDARD 11)
add_definitions(-std=c++11)

file(GLOB SOURCE_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(evdev-calibration ${SOURCE_FILES})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIR}
                    ${SDL2_TTF_INCLUDE_DIR})
target_link_libraries(evdev-calibration ${SDL2_LIBRARY}
                             ${SDL2_TTF_LIBRARIES})