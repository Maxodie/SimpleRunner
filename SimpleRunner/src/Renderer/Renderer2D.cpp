#include "Renderer/Renderer2D.hpp"
#include "Renderer/RendererAPI.hpp"
#include "nvrhi/nvrhi.h"

namespace SR
{

Renderer2D::Data Renderer2D::s_data;

void Renderer2D::Init()
{
    s_data.CommandList.Create(RendererAPI::GetContext());
    s_data.QuadVertexBuffer.Create(RendererAPI::GetContext(), sizeof(s_data.g_Vertices));

    CORE_LOG_SUCCESS("Renderer2D initialized");
}

void Renderer2D::Shutdown()
{
    s_data.QuadVertexBuffer.Destroy();
    s_data.CommandList.Destroy();

    CORE_LOG_SUCCESS("Renderer2D has been shutdown");
}

void Renderer2D::Begin()
{

    //UNIFORM LAYOUT DESC BINDING HANDLE
    // Note: the binding set must include all bindings declared in the layout, and nothing else.
    // This condition is tested by the validation layer.
    // The order of items in the binding set doesn't matter.
    // auto bindingSetDesc = nvrhi::BindingSetDesc()
    //     .addItem(nvrhi::BindingSetItem::Texture_SRV(0, m_data.GeometryTexture.GetHandle()))
    //     .addItem(nvrhi::BindingSetItem::ConstantBuffer(0, m_data.ConstantBuffer.GetHandle()));

    RendererAPI::BeginFrame(s_data.CommandList, nvrhi::Color(100));

    RendererAPI::SetGraphicsState(s_data.CommandList, s_data.QuadVertexBuffer);
}

void Renderer2D::End()
{
    s_data.CommandList.GetHandle()->close();
    RendererAPI::ExecuteCommandList(s_data.CommandList);

    RendererAPI::Present();
}

void Renderer2D::DrawQuad(glm::vec2 position, glm::vec2 scale)
{
    s_data.CommandList.GetHandle()->writeBuffer(
        s_data.QuadVertexBuffer.GetHandle(), s_data.g_Vertices, sizeof(s_data.g_Vertices)
    );
    // nvrhi::BindingSetHandle bindingSet = nvrhiDevice->createBindingSet(bindingSetDesc, bindingLayout);\

    //Writing texture buffer
    // RendererAPI::GetContext().CommandListHandle->writeTexture();

    auto drawArguments = nvrhi::DrawArguments()
        .setVertexCount(std::size(s_data.g_Vertices));
    s_data.CommandList.GetHandle()->draw(drawArguments);
}

}
