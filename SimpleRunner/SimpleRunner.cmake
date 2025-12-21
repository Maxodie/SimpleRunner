add_library(SimpleRunner
    SimpleRunner/src/SimpleRunner.hpp

        SimpleRunner/src/Core/Core.hpp
        SimpleRunner/src/Core/Application.hpp
        SimpleRunner/src/Core/Application.cpp
)

target_link_libraries(SimpleRunner PRIVATE
    glfw
    glm_math
    imgui
    StbImplementation
)

target_include_directories(SimpleRunner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleRunner/src
)

set_target_properties(SimpleRunner PROPERTIES
    OUTPUT_NAME SimpleRunner
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/SimpleRunner
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/SimpleRunner
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/SimpleRunner
)

if(COMPILE_CONFIG_BUILD)
    set(CONFIG_COMPILE_DEFINITION SR_BUILD)
else()
    set(CONFIG_COMPILE_DEFINITION SR_EDITOR)
endif()

message("==== Config compile definition : ${CONFIG_COMPILE_DEFINITION} ====")

target_compile_definitions(SimpleRunner PRIVATE
    SR_LOGGER
    SR_ASSERT

    ${CONFIG_COMPILE_DEFINITION}
)

