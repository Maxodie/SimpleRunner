#include "Renderer/Texture.hpp"
#include "Renderer/VulkanStructures.hpp"
#include "Renderer/RendererAPI.hpp"

namespace SR
{

void Texture::Create(const Data& data)
{
    Create(RendererAPI::GetContext(), data);
}

void Texture::Create(RendererContext& context, const Data& data)
{
    m_data = data;
    auto textureDesc = nvrhi::TextureDesc()
        .setDimension(nvrhi::TextureDimension::Texture2D)
        .setWidth(m_data.Width)
        .setHeight(m_data.Height)
        .setFormat(nvrhi::Format::SRGBA8_UNORM)
        .enableAutomaticStateTracking(nvrhi::ResourceStates::ShaderResource)
        .setDebugName("Geometry Texture");

    nvrhi::TextureHandle geometryTexture = context.DeviceHandle->createTexture(textureDesc);
}

}
