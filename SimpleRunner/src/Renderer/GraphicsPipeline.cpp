#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Vertex.hpp"

namespace SR
{

void GraphicsPipeline::Create(RendererContext& context)
{
    // Assume the shaders are included as C headers; they could just as well be loaded from files.
    const char g_VertexShader[] =
        "#version 450"
        "layout (location = 0) in vec3 iPosition;"
        "layout (location = 1) in vec2 iTexCoord;"
        "void main()"
        "{ gl_Position = iPosition; }";
    const char g_PixelShader[] =
        "#version 450"
        "layout (location = 0) out vec4 outColor;"
        "void main()"
        "{ outColor = vec4(1, 0, 0, 1); }";

    m_data.VertexShader = context.DeviceHandle->createShader(
        nvrhi::ShaderDesc().setShaderType(nvrhi::ShaderType::Vertex),
        g_VertexShader, sizeof(g_VertexShader)
    );

    nvrhi::VertexAttributeDesc attributes[] = {
        nvrhi::VertexAttributeDesc()
            .setName("POSITION")
            .setFormat(nvrhi::Format::RGB32_FLOAT)
            .setOffset(offsetof(Vertex, position))
            .setElementStride(sizeof(Vertex)),
        nvrhi::VertexAttributeDesc()
            .setName("TEXCOORD")
            .setFormat(nvrhi::Format::RG32_FLOAT)
            .setOffset(offsetof(Vertex, texCoord))
            .setElementStride(sizeof(Vertex)),
    };

    m_data.InputLayoutHandle = context.DeviceHandle->createInputLayout(
        attributes, uint32_t(std::size(attributes)), m_data.VertexShader
    );

    m_data.FragmentShader = context.DeviceHandle->createShader(
        nvrhi::ShaderDesc().setShaderType(nvrhi::ShaderType::Pixel),
        g_PixelShader, sizeof(g_PixelShader)
    );

    auto framebufferInfo = context.Framebuffer->getFramebufferInfo();
    framebufferInfo.addColorFormat(nvrhi::Format::RGBA8_UNORM);

    //UNIFORM LAYOUT DESC
    // auto layoutDesc = nvrhi::BindingLayoutDesc()
    //     .setVisibility(nvrhi::ShaderType::All)
    //     .addItem(nvrhi::BindingLayoutItem::Texture_SRV(0))             // texture at t0
    //     .addItem(nvrhi::BindingLayoutItem::VolatileConstantBuffer(0)); // constants at b0
    // nvrhi::BindingLayoutHandle bindingLayout = nvrhiDevice->createBindingLayout(layoutDesc);

    auto pipelineDesc = nvrhi::GraphicsPipelineDesc()
        .setInputLayout(m_data.InputLayoutHandle)
        .setVertexShader(m_data.VertexShader)
        .setPixelShader(m_data.FragmentShader);
        // .addBindingLayout(bindingLayout);

    m_data.GraphicsPipeline = context.DeviceHandle->createGraphicsPipeline(pipelineDesc, framebufferInfo);

    //UNIFORM LAYOUT DESC BINDING HANDLE
    // Note: the binding set must include all bindings declared in the layout, and nothing else.
    // This condition is tested by the validation layer.
    // The order of items in the binding set doesn't matter.
    // auto bindingSetDesc = nvrhi::BindingSetDesc()
    //     .addItem(nvrhi::BindingSetItem::Texture_SRV(0, m_data.GeometryTexture.GetHandle()))
    //     .addItem(nvrhi::BindingSetItem::ConstantBuffer(0, m_data.ConstantBuffer.GetHandle()));
    //
    // nvrhi::BindingSetHandle bindingSet = nvrhiDevice->createBindingSet(bindingSetDesc, bindingLayout);\


    CORE_LOG_SUCCESS("Vulkan Graphics pipeline created");
}

void GraphicsPipeline::Destroy(RendererContext& context)
{
    m_data.FragmentShader = nullptr;
    m_data.InputLayoutHandle = nullptr;
    m_data.VertexShader = nullptr;

    CORE_LOG_SUCCESS("Vulkan Graphics pipeline destroyed");
}

}
