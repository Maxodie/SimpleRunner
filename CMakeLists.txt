cmake_minimum_required(VERSION 3.30)
project(SimpleRunnerProj VERSION 1.0.0)
cmake_policy(SET CMP0072 NEW)

option(CMAKE_EXPORT_COMPILE_COMMANDS "allow the creatation of an compile_commands.json file" ON)
option(GIT_SUBMODULE "allow git submodule auto update" ON)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    message("==== Create .clangd file to store 'CompilationDatabase: build/${CMAKE_BUILD_TYPE}' ====")
    file(WRITE ".clangd"
        "CompileFlags:
            CompilationDatabase: build/${CMAKE_BUILD_TYPE}"
    )
endif()

if(GIT_SUBMODULE)
    find_package(Git REQUIRED)
    message(STATUS "Submodule update")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMO_RESULT)
    if(NOT GIT_SUBMO_RESULT EQUAL 0)
        message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMO_RESULT}")
    endif()
endif()

set(BIN_OUTPUT_DIR "/bin/${CMAKE_SYSTEM_NAME}")
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}${BIN_OUTPUT_DIR}")

find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    target_link_options(Vulkan::shaderc_combined INTERFACE -Wl,-ignore:4099)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set_target_properties(Vulkan::shaderc_combined PROPERTIES LINK_FLAGS /ignore:4099)
endif()

message("Vulkan package version: ${Vulkan_VERSION}")

if(COMPILE_CONFIG_BUILD)
    set(CONFIG_COMPILE_DEFINITION SR_BUILD)
else()
    set(CONFIG_COMPILE_DEFINITION SR_EDITOR)
endif()

include(SimpleRunner/SimpleRunner.cmake)
include(SimpleRunnerEditor/SimpleRunnerEditor.cmake)
include(SimpleRunnerGame/SimpleRunnerGame.cmake)
include(SimpleRunner/vendors/GLFW/GLFW.cmake)
include(SimpleRunner/vendors/glm/glm.cmake)
include(SimpleRunner/vendors/imgui/imgui.cmake)
include(SimpleRunner/vendors/stb/stb.cmake)
include(SimpleRunner/vendors/NVRHI/NVRHI.cmake)

unset(COMPILE_CONFIG_BUILD CACHE)

