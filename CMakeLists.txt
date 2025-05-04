cmake_minimum_required(VERSION 3.25)
project(FECS)

set(CMAKE_CXX_STANDARD 23)

set(FECS_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/includes)
include_directories(${FECS_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/vendor/)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -flto")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

add_executable(${PROJECT_NAME} main.cpp)
