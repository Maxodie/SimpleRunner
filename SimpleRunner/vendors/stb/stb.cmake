add_library(StbImplementation
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleRunner/vendors/stb/StbImplementation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleRunner/vendors/stb/StbImplementation.hpp
)

set_target_properties(StbImplementation PROPERTIES
    OUTPUT_NAME StbImplementation
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/StbImplementation
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/StbImplementation
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/StbImplementation
)

target_include_directories(StbImplementation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleRunner/vendors/stb/stb
)
