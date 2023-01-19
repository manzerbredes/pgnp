
# Copy asset files
file(COPY pgn_files DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# Configure catch3
include_directories(./catch3/)
add_library(pgnp_catch3 SHARED ./catch3/catch_amalgamated.cpp)

# Add tests
add_executable(pgnp_valid valid.cpp)
target_link_libraries(pgnp_valid pgnp pgnp_catch3 ChessMoveInterface)
add_test(PGNP_Valid_PGN_Set pgnp_valid)

add_executable(pgnp_str str.cpp)
target_link_libraries(pgnp_str pgnp pgnp_catch3 ChessMoveInterface)
add_test(PGNP_STR_Compliant_Set pgnp_str)

add_executable(pgnp_combined combined.cpp)
target_link_libraries(pgnp_combined pgnp pgnp_catch3 ChessMoveInterface)
add_test(PGNP_Combined_Set pgnp_combined)

add_executable(from_string from_string.cpp)
target_link_libraries(from_string pgnp pgnp_catch3 ChessMoveInterface)
add_test(PGNP_FromString_Set from_string)
