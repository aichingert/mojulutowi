cmake_minimum_required(VERSION 4.0 FATAL_ERROR)

project(bed LANGUAGES C)

file(GLOB_RECURSE C_SOURCE_FILES "src/*.h" "src/*.c")

set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic -Werror -lrt -lwayland-client")
add_subdirectory(../../lu ${CMAKE_BINARY_DIR}/lu)

add_executable(bed ${C_SOURCE_FILES})
target_link_libraries(bed PRIVATE lu)

