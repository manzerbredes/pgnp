include_directories(./catch3/)

# Copy asset files
file(COPY pgn_files DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# Run tests
add_executable(pgnp_tests tests.cpp ./catch3/catch_amalgamated.cpp)
target_link_libraries(pgnp_tests pgnp)
add_test(PGNP_Tests pgnp_tests)
