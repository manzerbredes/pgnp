cmake_minimum_required(VERSION 3.10)
project(pgnp)

# Shared library
add_library(pgnp SHARED src/pgnp.cpp)

# Includes
set(PGNP_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/includes) # For conveniance
set(PGNP_INCLUDE_DIR ${PGNP_INCLUDE_DIR} PARENT_SCOPE) # To be used by other projects with add_subdirectory()
file(MAKE_DIRECTORY ${PGNP_INCLUDE_DIR})
configure_file(src/pgnp.hpp ${PGNP_INCLUDE_DIR} COPYONLY)

# Unit tests
enable_testing()
add_subdirectory(./tests)
