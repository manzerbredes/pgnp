
# Copy asset files
file(COPY pgn_files DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# Configure catch3
include_directories(./catch3/)
add_library(catch3 SHARED ./catch3/catch_amalgamated.cpp)

# Add tests
add_executable(pgnp_valid valid.cpp)
target_link_libraries(pgnp_valid pgnp catch3)
add_test(PGNP_Valid_PGN_Set pgnp_valid)

add_executable(pgnp_str str.cpp)
target_link_libraries(pgnp_str pgnp catch3)
add_test(PGNP_STR_Compliant_Set pgnp_str)

add_executable(pgnp_combined combined.cpp)
target_link_libraries(pgnp_combined pgnp catch3)
add_test(PGNP_Combined_Set pgnp_combined)