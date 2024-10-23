cmake_minimum_required(VERSION 3.25)
project(FECS)

file (GLOB_RECURSE source
    "src/*.h"
    "src/*.cpp"
)

add_executable(${PROJECT_NAME} main.cpp ${source})
