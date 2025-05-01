cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 23)

set(FECS_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/includes)
include_directories(${FECS_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/vendor/)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

add_subdirectory(examples/benchmark/)
