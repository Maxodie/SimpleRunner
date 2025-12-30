add_library(SimpleRunner
    SimpleRunner/src/SimpleRunner.hpp
    SimpleRunner/src/EntryPoint.hpp
    SimpleRunner/src/EntryPoint.cpp

        SimpleRunner/src/Core/Core.hpp
        SimpleRunner/src/Core/Application.hpp
        SimpleRunner/src/Core/Application.cpp

        SimpleRunner/src/Event/Event.hpp

        SimpleRunner/src/LayerStack/LayerStack.hpp
        SimpleRunner/src/LayerStack/LayerStack.cpp

        SimpleRunner/src/Window/Window.hpp
        SimpleRunner/src/Window/Window.cpp
        SimpleRunner/src/Window/GraphicsContext.hpp
        SimpleRunner/src/Window/GraphicsContext.cpp

        SimpleRunner/src/Log/Log.hpp
        SimpleRunner/src/Log/Log.cpp

        SimpleRunner/src/Renderer/VulkanInclude.hpp
        SimpleRunner/src/Renderer/VulkanStructures.hpp
        SimpleRunner/src/Renderer/RendererAPI.hpp
        SimpleRunner/src/Renderer/RendererAPI.cpp
        SimpleRunner/src/Renderer/SwapChain.hpp
        SimpleRunner/src/Renderer/SwapChain.cpp
        SimpleRunner/src/Renderer/GraphicsPipeline.hpp
        SimpleRunner/src/Renderer/GraphicsPipeline.cpp
        SimpleRunner/src/Renderer/Buffer.hpp
        SimpleRunner/src/Renderer/Buffer.cpp
        SimpleRunner/src/Renderer/Texture.hpp
        SimpleRunner/src/Renderer/Texture.cpp
        SimpleRunner/src/Renderer/Vertex.hpp
        SimpleRunner/src/Renderer/Renderer2D.hpp
        SimpleRunner/src/Renderer/Renderer2D.cpp
)

target_link_libraries(SimpleRunner PUBLIC
    glfw
    glm_math
    imgui
    StbImplementation
    Vulkan::Vulkan
    Vulkan::shaderc_combined
    nvrhi
    nvrhi_vk
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

message("==== Config compile definition : ${CONFIG_COMPILE_DEFINITION} ====")

target_compile_definitions(SimpleRunner PRIVATE
    "$<$<CONFIG:debug>:SR_LOGGER>"
    "$<$<CONFIG:debug>:SR_ASSERT>"

    ${CONFIG_COMPILE_DEFINITION}
)

