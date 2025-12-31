#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Vertex.hpp"
#include <shaderc/shaderc.hpp>

namespace SR
{

#define SR_CHECK_SHADER(shader, type, content) if(shader.GetCompilationStatus() != shaderc_compilation_status_success)\
{\
    CORE_LOG_ERROR("failed to compile shaders %s code: %d, error %s, content: %s",\
                   type, shader.GetCompilationStatus(), shader.GetErrorMessage().c_str(), content);\
    return false;\
}

bool GraphicsPipeline::Create(RendererContext& context, const nvrhi::FramebufferInfo& info)
{

    // Assume the shaders are included as C headers; they could just as well be loaded from files.
    const char g_VertexShader[] =
        "#version 450\n"
        "layout (location = 0) in vec3 iPosition;\n"
        "layout (location = 1) in vec2 iTexCoord;\n"
        "layout (location = 2) in vec4 iColor;\n"
        "layout (location = 0) out vec4 oiColor;\n"
        "void main()\n"
        "{ gl_Position = vec4(iPosition, 1);\n"
        " oiColor = iColor;\n"
        " }";
    const char g_PixelShader[] =
        "#version 450\n"
        "layout (location = 0) in vec4 oiColor;\n"
        "layout (location = 0) out vec4 outColor;\n"
        "void main()\n"
        "{ outColor = oiColor; }\n";

    shaderc::Compiler compiler;
    shaderc::SpvCompilationResult vertexResult = compiler.CompileGlslToSpv(
        g_VertexShader, sizeof(g_VertexShader)-1,
        shaderc_shader_kind::shaderc_glsl_vertex_shader,
        "vertex",
        shaderc::CompileOptions()
    );

    shaderc::SpvCompilationResult fragmentResult = compiler.CompileGlslToSpv(
        g_PixelShader, sizeof(g_PixelShader)-1,
        shaderc_shader_kind::shaderc_glsl_fragment_shader,
        "fragment",
        shaderc::CompileOptions()
    );

    SR_CHECK_SHADER(vertexResult, "vertex", g_VertexShader)
    SR_CHECK_SHADER(fragmentResult, "fragment", g_PixelShader)

    m_data.VertexShader = context.GetHandle()->createShader(
        nvrhi::ShaderDesc().setShaderType(nvrhi::ShaderType::Vertex),
        vertexResult.cbegin(), (vertexResult.cend() - vertexResult.cbegin()) * sizeof(uint32_t)
    );

    nvrhi::VertexAttributeDesc attributes[] = {
        nvrhi::VertexAttributeDesc()
            .setName("iPosition")
            .setFormat(nvrhi::Format::RGB32_FLOAT)
            .setOffset(offsetof(Vertex, position))
            .setElementStride(sizeof(Vertex)),
        nvrhi::VertexAttributeDesc()
            .setName("iTexCoord")
            .setFormat(nvrhi::Format::RG32_FLOAT)
            .setOffset(offsetof(Vertex, texCoord))
            .setElementStride(sizeof(Vertex)),
        nvrhi::VertexAttributeDesc()
            .setName("iColor")
            .setFormat(nvrhi::Format::RGBA32_FLOAT)
            .setOffset(offsetof(Vertex, color))
            .setElementStride(sizeof(Vertex)),
    };

    m_data.InputLayoutHandle = context.GetHandle()->createInputLayout(
        attributes, uint32_t(std::size(attributes)), m_data.VertexShader
    );

    m_data.FragmentShader = context.GetHandle()->createShader(
        nvrhi::ShaderDesc().setShaderType(nvrhi::ShaderType::Pixel),
        fragmentResult.cbegin(), (fragmentResult.cend() - fragmentResult.cbegin()) * sizeof(uint32_t)
    );

    //UNIFORM LAYOUT DESC
    // auto layoutDesc = nvrhi::BindingLayoutDesc()
    //     .setVisibility(nvrhi::ShaderType::All)
    //     .addItem(nvrhi::BindingLayoutItem::Texture_SRV(0))             // texture at t0
    //     .addItem(nvrhi::BindingLayoutItem::VolatileConstantBuffer(0)); // constants at b0
    // nvrhi::BindingLayoutHandle bindingLayout = nvrhiDevice->createBindingLayout(layoutDesc);

    auto pipelineDesc = nvrhi::GraphicsPipelineDesc()
        .setInputLayout(m_data.InputLayoutHandle)
        .setVertexShader(m_data.VertexShader)
        .setPixelShader(m_data.FragmentShader)
        .setRenderState(nvrhi::RenderState().setDepthStencilState(
                nvrhi::DepthStencilState()
                .disableDepthTest()
                .disableDepthWrite()
                .disableStencil()
            )
            .setRasterState(nvrhi::RasterState().setFrontCounterClockwise(false)
                .setCullBack())
        );
        // .addBindingLayout(bindingLayout);

    m_data.GraphicsPipeline = context.GetHandle()->createGraphicsPipeline(pipelineDesc, info);
    if(!m_data.GraphicsPipeline)
    {
        return false;
    }


    CORE_LOG_SUCCESS("Vulkan Graphics pipeline created");
    return true;
}

void GraphicsPipeline::Destroy(RendererContext& context)
{
    m_data.GraphicsPipeline = nullptr;
    m_data.FragmentShader = nullptr;
    m_data.InputLayoutHandle = nullptr;
    m_data.VertexShader = nullptr;

    CORE_LOG_SUCCESS("Vulkan Graphics pipeline destroyed");
}

}
