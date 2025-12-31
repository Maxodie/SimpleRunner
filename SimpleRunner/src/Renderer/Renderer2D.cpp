#include "Renderer/Renderer2D.hpp"
#include "Renderer/RendererAPI.hpp"
#include "nvrhi/nvrhi.h"
#include "nvrhi/utils.h"

namespace SR
{

Renderer2D::Data Renderer2D::s_data;

void Renderer2D::Init()
{
    s_data.CommandList.Create(RendererAPI::GetContext());

    s_data.QuadVertexBuffer.Create(RendererAPI::GetContext(), sizeof(s_data.Vertices));

    s_data.QuadIndexBuffer.Create(
        RendererAPI::GetContext(),
        sizeof(s_data.Indices) / std::size(s_data.Indices),
        std::size(s_data.Indices)
    );

    //setup default buffers
    s_data.CommandList.GetHandle()->open();

    s_data.CommandList.GetHandle()->writeBuffer(
        s_data.QuadVertexBuffer.GetHandle(), s_data.Vertices, sizeof(s_data.Vertices)
    );

    s_data.CommandList.GetHandle()->writeBuffer(
        s_data.QuadIndexBuffer.GetHandle(), s_data.Indices, sizeof(s_data.Indices)
    );
    s_data.CommandList.GetHandle()->close();
    RendererAPI::ExecuteCommandList(s_data.CommandList);
    //setup default buffers

    CORE_LOG_SUCCESS("Renderer2D initialized");
}

void Renderer2D::Shutdown()
{
    s_data.QuadIndexBuffer.Destroy();
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

    RendererAPI::BeginFrame(s_data.CommandList, nvrhi::Color(0.1f, 0.1f, 0.1f, 1.0f));
}

void Renderer2D::End()
{
    RendererAPI::EndFrame(s_data.CommandList);
    RendererAPI::Present();
}

void Renderer2D::DrawQuad(glm::vec2 position, glm::vec2 scale)
{
    s_data.CommandList.GetHandle()->beginMarker("Draw quad");

    // nvrhi::BindingSetHandle bindingSet = nvrhiDevice->createBindingSet(bindingSetDesc, bindingLayout);\

    //Writing texture buffer
    // RendererAPI::GetContext().CommandListHandle->writeTexture();

    RendererAPI::SetGraphicsState(s_data.CommandList, s_data.QuadVertexBuffer, s_data.QuadIndexBuffer);

    auto drawArguments = nvrhi::DrawArguments()
        .setStartIndexLocation(0)
        .setInstanceCount(1)
        .setStartVertexLocation(0)
        .setVertexCount(std::size(s_data.Indices));
    s_data.CommandList.GetHandle()->drawIndexed(drawArguments);

    s_data.CommandList.GetHandle()->endMarker();
}

}
