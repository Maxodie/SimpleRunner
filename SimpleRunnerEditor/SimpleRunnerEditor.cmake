if(CONFIG_COMPILE_DEFINITION STREQUAL SR_EDITOR)
    add_executable(SimpleRunnerEditor
        SimpleRunnerEditor/src/Editor.cpp
    )

    target_link_libraries(SimpleRunnerEditor PRIVATE
        SimpleRunner
    )

    target_include_directories(SimpleRunnerEditor PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/SimpleRunner/src
        ${CMAKE_CURRENT_SOURCE_DIR}/SimpleRunnerEditor/src
    )

    set_target_properties(SimpleRunnerEditor PROPERTIES
        OUTPUT_NAME SimpleRunnerEditor
        ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/SimpleRunnerEditor
        LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/SimpleRunnerEditor
        RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/SimpleRunnerEditor
    )
endif()

