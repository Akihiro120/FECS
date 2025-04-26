cmake_minimum_required(VERSION 3.25)
project(FECS)

set(CMAKE_CXX_STANDARD 23)

set(FECS_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/includes)
include_directories(${FECS_INCLUDE_DIRS})

file (GLOB_RECURSE FECS_SOURCES
	 src/*.cpp
)
# add_library(FECS ${FECS_SOURCES})
add_executable(FECS main.cpp ${FECS_SOURCES})

target_include_directories(FECS PUBLIC ${FECS_INCLUDE_DIRS})
