cmake_minimum_required(VERSION 3.25)
project(FECS)

set(CMAKE_CXX_STANDARD 23)

set(FECS_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/includes)
include_directories(${FECS_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

# file (GLOB_RECURSE FECS_SOURCES
# 	 src/*.cpp
# )
# add_library(FECS STATIC ${FECS_SOURCES})

# target_include_directories(FECS PUBLIC ${FECS_INCLUDE_DIRS})

if (${FECS_BUILD_EXAMPLES})
    add_subdirectory(examples/benchmark/)
endif ()
