cmake_minimum_required(VERSION 3.25)
project(FECS 
    VERSION 4.0
    DESCRIPTION "Functional Entity Component System"
    LANGUAGES CXX
)

# ============================================================
# Global Settings
# ============================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)   # Hard-fail if C++20 isn't available
set(CMAKE_CXX_EXTENSIONS OFF)         # Use -std=c++20, not -std=gnu++20

# ============================================================
# Options
# ============================================================
option(FECS_BUILD_EXAMPLES  "Build the example executables for FECS" ON)
option(FECS_WARNINGS_AS_ERRORS "Treat compiler warnings as errors"   OFF)

# ============================================================
# Print configuration summary
# ============================================================
message(STATUS "--------------------------------------------")
message(STATUS "  FECS v${PROJECT_VERSION}")
message(STATUS "  Build type     : ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler       : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  C++ Standard   : ${CMAKE_CXX_STANDARD}")
message(STATUS "  Install prefix : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build examples : ${FECS_BUILD_EXAMPLES}")
# message(STATUS "  Build tests    : ${FECS_BUILD_TESTS}")
message(STATUS "--------------------------------------------")

# ============================================================
# Library target
# ============================================================
set(PROJECT_SOURCES
    src/FECS/World/World.cpp
    src/FECS/Entity/EntityAllocator.cpp
    src/FECS/Entity.cpp
    src/FECS/Encoding/Encoder.cpp
)

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
add_library(FECS::FECS ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20) 

# ============================================================
# Compiler warnings
# ============================================================
include(CheckCXXCompilerFlag)

set(FECS_WARNING_FLAGS "")
if(MSVC)
    list(APPEND FECS_WARNING_FLAGS /W4)
    if(FECS_WARNINGS_AS_ERRORS)
        list(APPEND FECS_WARNING_FLAGS /WX)
    endif()
else()
    list(APPEND FECS_WARNING_FLAGS -Wall -Wextra -Wpedantic -Wshadow)
    if(FECS_WARNINGS_AS_ERRORS)
        list(APPEND FECS_WARNING_FLAGS -Werror)
    endif()
endif()

target_compile_options(${PROJECT_NAME} PRIVATE ${FECS_WARNING_FLAGS})

# ============================================================
# Subdirectories
# ============================================================
if(FECS_BUILD_EXAMPLES)
    message(STATUS "Building FECS examples...")
    add_subdirectory(examples/testing)
endif()
