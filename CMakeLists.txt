cmake_minimum_required(VERSION 3.10)
project(Oblivion VERSION 0.1 LANGUAGES CXX)

# set variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)  # For static libraries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)  # For shared libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  # For executables

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") # default flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -pedantic")
endif()

# include directories
include_directories(include)
include_directories(lib)

# create library "Oblivion"
file(GLOB_RECURSE
    OBLIVION_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/oblivion/*.hpp" "${CMAKE_SOURCE_DIR}/src/oblivion/*.cpp")
add_library(Oblivion ${OBLIVION_SOURCES})

# add main executable
add_executable(oblivion-cli "${CMAKE_SOURCE_DIR}/exe/main.cpp")
target_link_libraries(oblivion-cli PUBLIC Oblivion)
add_custom_target(run
    COMMAND ./oblivion-cli
    DEPENDS oblivion-cli
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
