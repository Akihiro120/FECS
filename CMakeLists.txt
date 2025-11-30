cmake_minimum_required(VERSION 3.25)
project(FECS LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 23)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

add_library(${PROJECT_NAME} STATIC src/FECS/World.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(MSVC)
    target_compile_options(FECS PRIVATE /O2 /Ot)
else()
    target_compile_options(FECS PRIVATE -O3 -march=native -ffast-math)
endif()

include(CheckCXXCompilerFlag)
if (PROJECT_IS_TOP_LEVEL)
    message(STATUS "Building FECS as top-level project. Adding examples.")
    
    add_subdirectory(examples/benchmark)
    add_subdirectory(examples/scheduling)
    add_subdirectory(examples/tests)
    add_subdirectory(examples/boids)
endif()
