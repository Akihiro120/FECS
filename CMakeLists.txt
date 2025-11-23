cmake_minimum_required(VERSION 3.25)
project(FECS LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 23)
add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_SOURCE_DIR}/vendor
)

include(CheckCXXCompilerFlag)
if (PROJECT_IS_TOP_LEVEL)
    message(STATUS "Building FECS as top-level project. Adding examples.")
    
    add_subdirectory(examples/benchmark)
    add_subdirectory(examples/tests)
    add_subdirectory(examples/querying)
endif()
