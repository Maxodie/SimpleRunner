#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Vertex.hpp"

namespace SR
{

bool GraphicsPipeline::Create(RendererContext& context, const ShaderLib& shaderLib, const nvrhi::FramebufferInfo& info)
{
    nvrhi::ShaderHandle vertexShaderHandle = nullptr;

    //according to createInputLayout only D3D11 should needs a vertexShader
#ifdef SR_WITH_D3D11
    const auto& vertexShader = std::find_if(
            shaderLib.GetShaders().begin(), shaderLib.GetShaders().end(),
            [](const ShaderLib::Shader& shader)
            {
                return shader.GetType() == ShaderType::VERTEX;
            }
        );
    if(vertexShader != shaderLib.GetShaders().end())
    {
        vertexShaderHandle = vertexShader->GetHandle();
    }
#endif

    m_data.InputLayoutHandle = context.GetHandle()->createInputLayout(
        Vertex::GetAttributes().data(), uint32_t(std::size(Vertex::GetAttributes())),
        vertexShaderHandle
    );

    //UNIFORM LAYOUT DESC
    // auto layoutDesc = nvrhi::BindingLayoutDesc()
    //     .setVisibility(nvrhi::ShaderType::All)
    //     .addItem(nvrhi::BindingLayoutItem::Texture_SRV(0))             // texture at t0
    //     .addItem(nvrhi::BindingLayoutItem::VolatileConstantBuffer(0)); // constants at b0
    // nvrhi::BindingLayoutHandle bindingLayout = nvrhiDevice->createBindingLayout(layoutDesc);

    auto pipelineDesc = nvrhi::GraphicsPipelineDesc()
        .setInputLayout(m_data.InputLayoutHandle)
        // .addBindingLayout(bindingLayout);
        .setRenderState(nvrhi::RenderState().setDepthStencilState(
                nvrhi::DepthStencilState()
                .disableDepthTest()
                .disableDepthWrite()
                .disableStencil()
            )
            .setRasterState(nvrhi::RasterState().setFrontCounterClockwise(false)
                .setCullBack())
        );

    for(const ShaderLib::Shader& shader : shaderLib.GetShaders())
    {
        if(shader.GetType() == ShaderType::VERTEX)
        {
            pipelineDesc.setVertexShader(shader.GetHandle());
        }
        else if(shader.GetType() == ShaderType::PIXEL)
        {
            pipelineDesc.setPixelShader(shader.GetHandle());
        }
        else
        {
            CORE_LOG_ERROR(
                "Shader graphics does not handle shader type %zu at the moment. Shader will be ignored",
                shader.GetType());
        }

    }

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
    m_data.InputLayoutHandle = nullptr;

    CORE_LOG_SUCCESS("Vulkan Graphics pipeline destroyed");
}

}
