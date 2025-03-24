cmake_minimum_required(VERSION 3.25)
project(FECS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
set(CMAKE_C_FLAGS_RELEASE   "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

file (GLOB_RECURSE source
    "src/*.h"
    "src/*.cpp"
)

add_executable(${PROJECT_NAME} main.cpp ${source})
