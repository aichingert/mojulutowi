cmake_minimum_required(VERSION 4.0 FATAL_ERROR)

project(lu LANGUAGES C)

file(GLOB_RECURSE C_SOURCE_FILES "*.h" "*.c")
add_library(lu ${C_SOURCE_FILES})
target_include_directories(lu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_C_FLAGS "-Werror -lrt -lwayland-client")

set(SCANNER "wayland-scanner")
set(XDG_SHELL_PATH "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml")
set(XDG_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(XDG_CODE "${XDG_OUTPUT_DIR}/xdg-shell.c")
set(XDG_HEADER "${XDG_OUTPUT_DIR}/xdg-shell.h")
set(XDG_CODE_COMMAND private-code < ${XDG_SHELL_PATH} > ${XDG_CODE})
set(XDG_HEADER_COMMAND client-header < ${XDG_SHELL_PATH} > ${XDG_HEADER})

add_custom_command(
    OUTPUT ${XDG_CODE}
    COMMAND ${SCANNER} ${XDG_CODE_COMMAND})
add_custom_command(
    OUTPUT ${XDG_HEADER}
    COMMAND ${SCANNER} ${XDG_HEADER_COMMAND})

add_custom_target(generate_xdg_shell DEPENDS ${XDG_CODE} ${XDG_HEADER})
add_dependencies(lu generate_xdg_shell)
target_sources(lu PRIVATE ${XDG_CODE} ${XDG_HEADER})

