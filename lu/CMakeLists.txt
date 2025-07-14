cmake_minimum_required(VERSION 4.0 FATAL_ERROR)

project(lu LANGUAGES C)

file(GLOB_RECURSE C_SOURCE_FILES "*.h" "*.c")
file(GLOB_RECURSE GLSL_SOURCE_FILES "shaders/*.glsl")

add_library(lu ${C_SOURCE_FILES})
target_include_directories(lu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_C_FLAGS "-Wall -Wpedantic -lwayland-client -Wreturn-type -fsanitize=address,leak,undefined -fstack-protector")

# wayland header generation

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

# vulkan shader compilation

set(GLSL_VALIDATOR "glslangValidator")
set(GLSL_FLAGS --target-env vulkan1.4)
set(SPIRV_OUTPUT_DIR "${PROJECT_BINARY_DIR}/spirv/")

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(STEM ${GLSL} NAME_WLE)
    set(SPIRV "${SPIRV_OUTPUT_DIR}${STEM}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SPIRV_OUTPUT_DIR}"
        COMMAND ${GLSL_VALIDATOR} -V ${GLSL_FLAGS} --quiet ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach()

add_custom_target(compile_shaders DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(lu compile_shaders)




