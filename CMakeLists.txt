cmake_minimum_required(VERSION 3.30)
project(SimpleRunnerProj VERSION 1.0.0)
cmake_policy(SET CMP0072 NEW)

option(CMAKE_EXPORT_COMPILE_COMMANDS "allow the creatation of an compile_commands.json file" ON)
option(GIT_SUBMODULE "allow git submodule auto update" ON)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    message("==== Create .clang file to store 'CompilationDatabase: build/' ====")
    file(WRITE ".clang"
        "CompileFlags:
            CompilationDatabase: build/"
    )
endif()

if(GIT_SUBMODULE)
    message(STATUS "Submodule update")
    execute_process(COMMAND git submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    COMMAND_ECHO STDOUT)
endif()

set(BIN_OUTPUT_DIR "/bin/${CMAKE_SYSTEM_NAME}")
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}${BIN_OUTPUT_DIR}")

find_package(Vulkan REQUIRED)

include(SimpleRunner/SimpleRunner.cmake)
include(SimpleRunnerEditor/SimpleRunnerEditor.cmake)
include(SimpleRunnerGame/SimpleRunnerGame.cmake)
include(SimpleRunner/vendors/GLFW/GLFW.cmake)
include(SimpleRunner/vendors/glm/glm.cmake)
include(SimpleRunner/vendors/imgui/imgui.cmake)
include(SimpleRunner/vendors/stb/stb.cmake)
include(SimpleRunner/vendors/NVRHI/NVRHI.cmake)

unset(COMPILE_CONFIG_BUILD CACHE)

