cmake_minimum_required(VERSION 3.25)
project(FECS)

set(CMAKE_CXX_STANDARD 23)

set(FECS_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/includes)
include_directories(${FECS_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/vendor/)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -flto")

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${FECS_INCLUDE_DIRS})

add_subdirectory(examples/benchmark)
add_subdirectory(examples/tests)
add_subdirectory(examples/querying)
