cmake_minimum_required(VERSION 3.10)
project(pgnp)

# Shared library
add_library(pgnp SHARED src/PGN.cpp src/HalfMove.cpp src/LargeFileStream.cpp)

# Includes
set(PGNP_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/includes) # For conveniance
set(PGNP_INCLUDE_DIR ${PGNP_INCLUDE_DIR} PARENT_SCOPE) # To be used by other projects with add_subdirectory()
file(MAKE_DIRECTORY ${PGNP_INCLUDE_DIR})
configure_file(src/PGN.hpp ${PGNP_INCLUDE_DIR}/pgnp.hpp COPYONLY)
configure_file(src/HalfMove.hpp ${PGNP_INCLUDE_DIR} COPYONLY)
configure_file(src/LargeFileStream.hpp ${PGNP_INCLUDE_DIR} COPYONLY)
configure_file(src/Types.hpp ${PGNP_INCLUDE_DIR} COPYONLY)
include_directories(${PGNP_INCLUDE_DIR})

# ChessMoveInterface
add_subdirectory(libs/chess-move-interface)
include_directories(${CMI_INCLUDE_DIR})
target_link_libraries(pgnp PUBLIC ChessMoveInterface)

# Add include directiries to target to be independent of ${PGNP_INCLUDE_DIR} and ${CMI_INCLUDE_DIR}
target_include_directories(pgnp PUBLIC ${CMI_INCLUDE_DIR})
target_include_directories(pgnp PUBLIC ${PGNP_INCLUDE_DIR})

# Unit tests
set(COMPILE_TESTS OFF CACHE BOOL "Should unit tests be compiled")
if(COMPILE_TESTS)
    enable_testing()
    add_subdirectory(./tests)
endif()

