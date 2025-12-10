cmake_minimum_required(VERSION 3.25)
project(FECS LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} STATIC src/FECS/World.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

include(CheckCXXCompilerFlag)
option(FECS_BUILD_EXAMPLES "Build the example executables for FECS" OFF)
if (FECS_BUILD_EXAMPLES)
    message(STATUS "Building FECS examples. Adding examples.")
    
    add_subdirectory(examples/benchmark)
    add_subdirectory(examples/scheduling)
    add_subdirectory(examples/tests)
    # add_subdirectory(examples/boids)
endif()
