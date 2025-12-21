# set(CMAKE_SUPPRESS_REGENERATION true)
# set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set project name
project (imgui LANGUAGES CXX)

set(ImguiFolder ${CMAKE_CURRENT_SOURCE_DIR}/SimpleRunner/vendors/imgui/imgui)

# Set the source files to a list
set(
    source_list
    ${ImguiFolder}/imconfig.h
    ${ImguiFolder}/imgui.cpp
    ${ImguiFolder}/imgui.h
    ${ImguiFolder}/imgui_demo.cpp
    ${ImguiFolder}/imgui_draw.cpp
    ${ImguiFolder}/imgui_internal.h
    ${ImguiFolder}/imgui_tables.cpp
    ${ImguiFolder}/imgui_widgets.cpp
    ${ImguiFolder}/imstb_rectpack.h
    ${ImguiFolder}/imstb_textedit.h
    ${ImguiFolder}/imstb_truetype.h

    ${ImguiFolder}/backends/imgui_impl_glfw.h
    ${ImguiFolder}/backends/imgui_impl_glfw.cpp
    ${ImguiFolder}/backends/imgui_impl_opengl3.h
    ${ImguiFolder}/backends/imgui_impl_opengl3.cpp

    ${ImguiFolder}/misc/cpp/imgui_stdlib.h
    ${ImguiFolder}/misc/cpp/imgui_stdlib.cpp
)

# Create executable file as project name
add_library(imgui ${source_list})

target_link_libraries(imgui PRIVATE
    glfw
)

set_target_properties(imgui PROPERTIES
    OUTPUT_NAME imgui
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/imgui
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/imgui
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/imgui
)

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleRunner/vendors/imgui/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleRunner/vendors/GLFW/glfw/include
)

# Create folder structure
# foreach(source IN LISTS source_list)
#     source_group("Source" FILES "${source}")
# endforeach()

