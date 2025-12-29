#include "Renderer/Renderer2D.hpp"
#include "Renderer/RendererAPI.hpp"
#include "nvrhi/utils.h"

namespace SR
{

Renderer2D::Data Renderer2D::s_data;

void Renderer2D::Init()
{
    s_data.CommandList.Create(RendererAPI::GetContext());

    CORE_LOG_SUCCESS("Renderer2D initialized");
}

void Renderer2D::Shutdown()
{
    s_data.CommandList.Destroy();

    CORE_LOG_SUCCESS("Renderer2D has been shutdown");
}

void Renderer2D::Begin()
{
    s_data.CommandList.GetHandle()->open();
    nvrhi::utils::ClearColorAttachment(
        s_data.CommandList.GetHandle(), RendererAPI::GetContext().Framebuffer, 0, nvrhi::Color(0)
    );

    RendererAPI::SetGraphicsState(s_data.CommandList, s_data.QuadVertexBuffer);
}

void Renderer2D::End()
{
    s_data.CommandList.GetHandle()->close();
    RendererAPI::ExecuteCommandList(s_data.CommandList);
}

void Renderer2D::DrawQuad(glm::vec2 position, glm::vec2 scale)
{
    s_data.CommandList.GetHandle()->writeBuffer(
        s_data.QuadVertexBuffer.GetHandle(), s_data.g_Vertices, sizeof(s_data.g_Vertices)
    );

    //Writing texture buffer
    // RendererAPI::GetContext().CommandListHandle->writeTexture();

    auto drawArguments = nvrhi::DrawArguments()
        .setVertexCount(std::size(s_data.g_Vertices));
    s_data.CommandList.GetHandle()->draw(drawArguments);
}

}
