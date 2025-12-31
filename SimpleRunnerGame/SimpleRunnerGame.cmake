if(CONFIG_COMPILE_DEFINITION STREQUAL SR_BUILD)
    add_executable(SimpleRunnerGame
        SimpleRunnerGame/src/Game.cpp
    )

    target_link_libraries(SimpleRunnerGame PRIVATE
        SimpleRunner
    )

    target_include_directories(SimpleRunnerGame PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/SimpleRunnerGame/src
        ${CMAKE_CURRENT_SOURCE_DIR}/SimpleRunner/src
    )

    set_target_properties(SimpleRunnerGame PROPERTIES
        OUTPUT_NAME SimpleRunnerGame
        ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/SimpleRunnerGame
        LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/SimpleRunnerGame
        RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/SimpleRunnerGame
    )
endif()


